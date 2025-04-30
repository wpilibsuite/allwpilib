// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <wpi/mutex.h>
#include <wpi/print.h>
#include <wpi/spinlock.h>
#include <wpigui.h>

#include "cscore.h"
#include "cscore_cv.h"

namespace gui = wpi::gui;

int main() {
  wpi::mutex latestFrameMutex;
  std::unique_ptr<cv::Mat> latestFrame;
  wpi::spinlock freeListMutex;
  std::vector<std::unique_ptr<cv::Mat>> freeList;
  std::atomic<bool> stopCamera{false};

  cs::UsbCamera camera{"usbcam", 0};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 640, 480, 30);
  cs::CvSink cvsink{"cvsink"};
  cvsink.SetSource(camera);

  std::thread thr([&] {
    cv::Mat frame;
    while (!stopCamera) {
      // get frame from camera
      uint64_t time = cvsink.GrabFrame(frame);
      if (time == 0) {
        wpi::print("error: {}\n", cvsink.GetError());
        continue;
      }

      // get or create a mat
      std::unique_ptr<cv::Mat> out;
      {
        std::scoped_lock lock{freeListMutex};
        if (!freeList.empty()) {
          out = std::move(freeList.back());
          freeList.pop_back();
        } else {
          out = std::make_unique<cv::Mat>();
        }
      }

      // convert to RGBA
      cv::cvtColor(frame, *out, cv::COLOR_BGR2RGBA);

      {
        // make available
        std::scoped_lock lock{latestFrameMutex};
        latestFrame.swap(out);
      }

      // put the previous frame on free list
      if (out) {
        std::scoped_lock lock{freeListMutex};
        freeList.emplace_back(std::move(out));
      }
    }
  });

  gui::CreateContext();
  gui::Initialize("Hello World", 1024, 768);
  gui::Texture tex;
  gui::AddEarlyExecute([&] {
    std::unique_ptr<cv::Mat> frame;
    {
      std::scoped_lock lock{latestFrameMutex};
      latestFrame.swap(frame);
    }
    if (frame) {
      // create or update texture
      if (!tex || frame->cols != tex.GetWidth() ||
          frame->rows != tex.GetHeight()) {
        tex = gui::Texture(gui::kPixelRGBA, frame->cols, frame->rows,
                           frame->data);
      } else {
        tex.Update(frame->data);
      }
      {
        std::scoped_lock lock{freeListMutex};
        freeList.emplace_back(std::move(frame));
      }
    }

    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Video")) {
      // render to window (best fit)
      if (tex && tex.GetWidth() != 0 && tex.GetHeight() != 0) {
        auto drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 imageMin = ImGui::GetWindowContentRegionMin();
        ImVec2 imageMax = ImGui::GetWindowContentRegionMax();
        gui::MaxFit(&imageMin, &imageMax, tex.GetWidth(), tex.GetHeight());
        drawList->AddImage(tex, windowPos + imageMin, windowPos + imageMax);
      }
    }
    ImGui::End();
  });
  gui::Main();
  stopCamera = true;
  thr.join();
}
