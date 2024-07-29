// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <thread>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <wpi/print.h>
#include <wpi/spinlock.h>
#include <wpigui.h>

#include "cscore.h"
#include "cscore_cv.h"

namespace gui = wpi::gui;

int main() {
  std::atomic<cv::Mat*> latestFrame{nullptr};
  std::vector<cv::Mat*> sharedFreeList;
  wpi::spinlock sharedFreeListMutex;
  std::vector<cv::Mat*> sourceFreeList;
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

      // get or create a mat, prefer sourceFreeList over sharedFreeList
      cv::Mat* out;
      if (!sourceFreeList.empty()) {
        out = sourceFreeList.back();
        sourceFreeList.pop_back();
      } else {
        {
          std::scoped_lock lock(sharedFreeListMutex);
          for (auto mat : sharedFreeList) {
            sourceFreeList.emplace_back(mat);
          }
          sharedFreeList.clear();
        }
        if (!sourceFreeList.empty()) {
          out = sourceFreeList.back();
          sourceFreeList.pop_back();
        } else {
          out = new cv::Mat;
        }
      }

      // convert to RGBA
      cv::cvtColor(frame, *out, cv::COLOR_BGR2RGBA);

      // make available
      auto prev = latestFrame.exchange(out);

      // put prev on free list
      if (prev) {
        sourceFreeList.emplace_back(prev);
      }
    }
  });

  gui::CreateContext();
  gui::Initialize("Hello World", 1024, 768);
  gui::Texture tex;
  gui::AddEarlyExecute([&] {
    auto frame = latestFrame.exchange(nullptr);
    if (frame) {
      // create or update texture
      if (!tex || frame->cols != tex.GetWidth() ||
          frame->rows != tex.GetHeight()) {
        tex = gui::Texture(gui::kPixelRGBA, frame->cols, frame->rows,
                           frame->data);
      } else {
        tex.Update(frame->data);
      }
      // put back on shared freelist
      std::scoped_lock lock(sharedFreeListMutex);
      sharedFreeList.emplace_back(frame);
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
