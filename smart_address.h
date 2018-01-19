/*
Copyright 2018 ko1N, zbe

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#ifndef __SMART_ADDRESS_H__
#define __SMART_ADDRESS_H__


#include <string.h>
#include <cstdint>
#include <memory>
#include <inttypes.h>


template <typename H, typename B>
class smart_address {

  class _writer {

    public:

      _writer(smart_address *address = nullptr) :
        m_address(address) { }

    public:

      template <typename T>
      _writer &operator = (const T &v) {
        this->m_address->ensure_capacity(sizeof(T));
        *(T *)(this->m_address->m_buffer.first.get()) = v;
        this->m_address->write(sizeof(T));
        return *this;
      }

      template <typename T>
      _writer &operator += (const T &v) {
        this->m_address->ensure_capacity(sizeof(T));
        this->m_address->read(sizeof(T));
        *(T *)(this->m_address->m_buffer.first.get()) += v;
        this->m_address->write(sizeof(T));
        return *this;
      }

      template <typename T>
      _writer &operator -= (const T &v) {
        this->m_address->ensure_capacity(sizeof(T));
        this->m_address->read(sizeof(T));
        *(T *)(this->m_address->m_buffer.first.get()) -= v;
        this->m_address->write(sizeof(T));
        return *this;
      }

    private:

      smart_address *m_address;

  };

  public:

    smart_address(void *handle) :
      m_handle(handle), m_address(0),
      m_writer(_writer(this)),
      m_buffer{ nullptr, 0 } { }

    smart_address(void *handle, const B &p) :
      smart_address(handle) {
      *this = p;
    }

    smart_address(const smart_address &p) :
      smart_address(p.m_handle) {
      this->m_address = p.m_address;
    }

  public:

    smart_address &operator = (const B &p) {
      this->m_address = p;
      return *this;
    }

    smart_address &operator += (const B &p) {
      this->m_address += p;
      return *this;
    }

    smart_address &operator -= (const B &p) {
      this->m_address -= p;
      return *this;
    }

    smart_address operator + (const B &p) {
      return smart_address(this->m_handle, this->m_address + p);
    }

    smart_address operator - (const B &p) {
      return smart_address(this->m_handle, this->m_address - p);
    }

    smart_address &operator += (const smart_address &p) {
      this->m_address += p.m_address;
      return *this;
    }

    smart_address &operator -= (const smart_address &p) {
      this->m_address -= p.m_address;
      return *this;
    }

    smart_address operator + (const smart_address &p) {
      return smart_address(this->m_handle, this->m_address + p.m_address);
    }

    smart_address operator - (const smart_address &p) {
      return smart_address(this->m_handle, this->m_address - p.m_address);
    }

    template <typename T>
    explicit operator T *() {
      this->ensure_capacity(sizeof(T));
      this->read(sizeof(T));
      return (T *)(this->m_buffer.first.get());
    }

    template <typename T>
    explicit operator T () {
      this->ensure_capacity(sizeof(T));
      this->read(sizeof(T));
      return *(T *)(this->m_buffer.first.get());
    }

    operator B () {
      return this->m_address;
    }

    _writer& operator *() {
      return this->m_writer;
    }

    H handle(void) {
      return this->m_handle;
    }

    B address(void) {
      return this->m_address;
    }

    _writer *writer(void) {
      return &this->m_writer;
    }

    auto buffer(void) {
      return this->m_buffer.first.get();
    }

    auto buffer_size(void) {
      return this->m_buffer.second;
    }

  protected:

    void ensure_capacity(size_t size) {
      if (!this->m_buffer.first || size > this->m_buffer.second) {
        this->m_buffer.first.reset(new uint8_t[this->m_buffer.second = size]);
      }
    }

    virtual void read(size_t size) {}
    virtual void write(size_t size) {}

  protected:

    H m_handle;
    B m_address;
    _writer m_writer;
    using buffer_t = std::pair< std::shared_ptr<uint8_t[]>, std::size_t >;
    buffer_t m_buffer;

};


#endif
