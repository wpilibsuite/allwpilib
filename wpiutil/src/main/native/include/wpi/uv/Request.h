/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_REQUEST_H_
#define WPIUTIL_WPI_UV_REQUEST_H_

#include <uv.h>

#include <functional>
#include <memory>

#include "wpi/uv/Error.h"

namespace wpi {
namespace uv {

/**
 * Request.  Requests are not moveable or copyable.
 * This class provides shared_ptr ownership and shared_from_this.
 */
class Request : public std::enable_shared_from_this<Request> {
 public:
  using Type = uv_req_type;

  Request(const Request&) = delete;
  Request(Request&&) = delete;
  Request& operator=(const Request&) = delete;
  Request& operator=(Request&&) = delete;
  virtual ~Request() noexcept = default;

  /**
   * Get the type of the request.
   *
   * A base request offers no functionality to promote it to the actual request
   * type. By means of this function, the type of the underlying request as
   * specified by Type is made available.
   *
   * @return The actual type of the request.
   */
  Type GetType() const noexcept { return m_uv_req->type; }

  /**
   * Get the name of the type of the request.  E.g. "connect" for connect.
   */
  const char* GetTypeName() const noexcept {
    return uv_req_type_name(m_uv_req->type);
  }

  /**
   * Cancel a pending request.
   *
   * This method fails if the request is executing or has finished
   * executing.
   * It can emit an error signal in case of errors.
   *
   * @return True in case of success, false otherwise.
   */
  bool Cancel() { return uv_cancel(m_uv_req) == 0; }

  /**
   * Return the size of the underlying request type.
   * @return The size of the underlying request type.
   */
  size_t RawSize() const noexcept { return uv_req_size(m_uv_req->type); }

  /**
   * Get the underlying request data structure.
   *
   * @return The underlying request data structure.
   */
  uv_req_t* GetRawReq() noexcept { return m_uv_req; }

  /**
   * Get the underlying request data structure.
   *
   * @return The underlying request data structure.
   */
  const uv_req_t* GetRawReq() const noexcept { return m_uv_req; }

  /**
   * Keep this request in memory even if no outside shared_ptr references
   * remain.  To release call Release().
   *
   * Derived classes can override this method for different memory management
   * approaches (e.g. pooled storage of requests).
   */
  virtual void Keep() noexcept { m_self = shared_from_this(); }

  /**
   * No longer force holding this request in memory.  Does not immediately
   * destroy the object unless no outside shared_ptr references remain.
   *
   * Derived classes can override this method for different memory management
   * approaches (e.g. pooled storage of requests).
   */
  virtual void Release() noexcept { m_self.reset(); }

  /**
   * Error callback.  By default, this is set up to report errors to the handle
   * that created this request.
   * @param err error code
   */
  std::function<void(Error)> error;

  /**
   * Report an error.
   * @param err Error code
   */
  void ReportError(int err) { error(Error(err)); }

 protected:
  /**
   * Constructor.
   */
  explicit Request(uv_req_t* uv_req) : m_uv_req{uv_req} {
    m_uv_req->data = this;
  }

 private:
  std::shared_ptr<Request> m_self;
  uv_req_t* m_uv_req;
};

/**
 * Request.  Requests are not moveable or copyable.
 * @tparam T CRTP derived class
 * @tparam U underlying libuv request type
 */
template <typename T, typename U>
class RequestImpl : public Request {
 public:
  std::shared_ptr<T> shared_from_this() {
    return std::static_pointer_cast<T>(this->shared_from_this());
  }

  std::shared_ptr<const T> shared_from_this() const {
    return std::static_pointer_cast<const T>(this->shared_from_this());
  }

  /**
   * Get the underlying request data structure.
   *
   * @return The underlying request data structure.
   */
  U* GetRaw() noexcept { return &m_uv_req; }

  /**
   * Get the underlying request data structure.
   *
   * @return The underlying request data structure.
   */
  const U* GetRaw() const noexcept { return &m_uv_req; }

 protected:
  /**
   * Constructor.
   */
  RequestImpl() : Request{reinterpret_cast<uv_req_t*>(&m_uv_req)} {}

 private:
  U m_uv_req;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_REQUEST_H_
