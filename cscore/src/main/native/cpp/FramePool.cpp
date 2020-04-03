/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "FramePool.h"

using namespace cs;

static constexpr size_t kMaxImagesAvail = 32;

FramePool::~FramePool() {
  // Set a flag so ReleaseFrame() doesn't re-add them to m_framesAvail.
  // Put in a block so we destroy before the destructor ends.
  {
    m_destroy = true;
    auto frames = std::move(m_framesAvail);
  }
}

std::unique_ptr<Image> FramePool::AllocImage(size_t size) {
  std::unique_ptr<Image> image;
  {
    std::scoped_lock lock{m_mutex};
    // find the smallest existing frame that is at least big enough.
    int found = -1;
    for (size_t i = 0; i < m_imagesAvail.size(); ++i) {
      // is it big enough?
      if (m_imagesAvail[i] && m_imagesAvail[i]->capacity() >= size) {
        // is it smaller than the last found?
        if (found < 0 ||
            m_imagesAvail[i]->capacity() < m_imagesAvail[found]->capacity()) {
          // yes, update
          found = i;
        }
      }
    }

    // if nothing found, allocate a new buffer
    if (found < 0)
      image.reset(new Image{size});
    else
      image = std::move(m_imagesAvail[found]);
  }

  // Initialize image
  image->SetSize(size);

  return image;
}

void FramePool::ReleaseImage(std::unique_ptr<Image> image) {
  std::scoped_lock lock{m_mutex};
  if (m_destroy) return;
  // Return the frame to the pool.  First try to find an empty slot, otherwise
  // add it to the end.
  auto it = std::find(m_imagesAvail.begin(), m_imagesAvail.end(), nullptr);
  if (it != m_imagesAvail.end()) {
    *it = std::move(image);
  } else if (m_imagesAvail.size() > kMaxImagesAvail) {
    // Replace smallest buffer; don't need to check for null because the above
    // find would have found it.
    auto it2 = std::min_element(
        m_imagesAvail.begin(), m_imagesAvail.end(),
        [](const std::unique_ptr<Image>& a, const std::unique_ptr<Image>& b) {
          return a->capacity() < b->capacity();
        });
    if ((*it2)->capacity() < image->capacity()) *it2 = std::move(image);
  } else {
    m_imagesAvail.emplace_back(std::move(image));
  }
}

std::unique_ptr<Frame::Impl> FramePool::AllocFrameImpl() {
  std::scoped_lock lock{m_mutex};

  if (m_framesAvail.empty()) return std::make_unique<Frame::Impl>(*this);

  auto impl = std::move(m_framesAvail.back());
  m_framesAvail.pop_back();
  return impl;
}

void FramePool::ReleaseFrameImpl(std::unique_ptr<Frame::Impl> impl) {
  for (auto image : impl->images) ReleaseImage(std::unique_ptr<Image>(image));
  impl->images.clear();
  std::scoped_lock lock{m_mutex};
  if (m_destroy) return;
  m_framesAvail.push_back(std::move(impl));
}
