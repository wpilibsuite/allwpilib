// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/CameraSource.h"

#include <fmt/format.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

using namespace glass;

wpi::sig::Signal<const char*, CameraSource*> CameraSource::sourceCreated;
wpi::StringMap<CameraSource*> CameraSource::sources;

CameraSource::~CameraSource() {
  Stop();
  if (m_frameThread.joinable()) {
    m_frameThread.join();
  }
  delete m_latestFrame.load();
  for (auto frame : m_sharedFreeList) {
    delete frame;
  }
  for (auto frame : m_sourceFreeList) {
    delete frame;
  }
}

void CameraSource::UpdateTexture() {
  // create or update texture when we get a new frame
  if (auto frame = m_latestFrame.exchange(nullptr)) {
    if (!m_tex || frame->cols != m_tex.GetWidth() ||
        frame->rows != m_tex.GetHeight()) {
      m_tex = wpi::gui::Texture(wpi::gui::kPixelRGBA, frame->cols, frame->rows,
                                frame->data);
    } else {
      m_tex.Update(frame->data);
    }
    // put back on shared freelist
    std::scoped_lock lock(m_sharedFreeListMutex);
    m_sharedFreeList.emplace_back(frame);
  }
}

void CameraSource::Start() {
  if (m_frameThread.joinable()) {
    return;
  }
  m_stopCamera = false;
  m_frameThread = std::thread([this, source = m_source] {
    cs::CvSink cvSink{fmt::format("{}_view", m_id), cs::VideoMode::kBGR};
    cvSink.SetSource(source);
    cv::Mat frame;
    while (!m_stopCamera) {
      // get frame from camera
      uint64_t time = cvSink.GrabFrame(frame, 0.25);
      if (m_stopCamera) {
        break;
      }

      cv::Mat* out = AllocMat();

      if (time == 0) {
        *out = cv::Mat::zeros(16, 16, CV_8UC4);
      } else {
        // convert to RGBA
        cv::cvtColor(frame, *out, cv::COLOR_BGR2RGBA);
      }

      // make available
      auto prev = m_latestFrame.exchange(out);

      // put prev on free list
      if (prev) {
        m_sourceFreeList.emplace_back(prev);
      }
    }
  });
}

void CameraSource::Stop() {
  m_stopCamera = true;
}

CameraSource* CameraSource::Find(std::string_view id) {
  auto it = sources.find(id);
  if (it == sources.end()) {
    return nullptr;
  }
  return it->second;
}

cv::Mat* CameraSource::AllocMat() {
  // get or create a mat, prefer sourceFreeList over sharedFreeList
  cv::Mat* out;
  if (!m_sourceFreeList.empty()) {
    out = m_sourceFreeList.back();
    m_sourceFreeList.pop_back();
  } else {
    {
      std::scoped_lock lock(m_sharedFreeListMutex);
      for (auto mat : m_sharedFreeList) {
        m_sourceFreeList.emplace_back(mat);
      }
      m_sharedFreeList.clear();
    }
    if (!m_sourceFreeList.empty()) {
      out = m_sourceFreeList.back();
      m_sourceFreeList.pop_back();
    } else {
      out = new cv::Mat;
    }
  }
  return out;
}
