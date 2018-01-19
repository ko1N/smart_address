# smart_address
smart address container for remote memory access

### requirements
at least g++ / msvc with c++14 support enabled

### how to use?
create your implementation:

example: win32 local process access
```c++
template <typename B>
class local_smart_address : public smart_address<void *, B> {

  public:

    using smart_address<void *, B>::smart_address;
    using smart_address<void *, B>::operator =;
    smart_address_win32(const smart_address<void *, B> &p) :
      smart_address<void *, B>(p) { }

  protected:

    virtual void read(size_t size) override {
      memcpy(this->m_buffer.first.get(), (void *)this->m_address, size);
    }

    virtual void write(size_t size) override {
      memcpy((void *)this->m_address, this->m_buffer.first.get(), size);
    }

};
```

example: win32 rpm/wpm with 64 bit addresses
```c++
class smart_address_win64 : public smart_address<HANDLE, uintptr_t> {

  public:

    using smart_address<HANDLE, uintptr_t>::smart_address;
    using smart_address<HANDLE, uintptr_t>::operator =;
    smart_address_win32(const smart_address<HANDLE, uintptr_t> &p) :
      smart_address<HANDLE, uintptr_t>(p) { }

  protected:

    virtual void read(size_t size) override {
      ReadProcessMemory(this->m_handle,
        this->m_address,
        this->m_buffer.first.get(),
        size);
    }

    virtual void write(size_t size) override {
      WriteProcessMemory(this->m_handle,
        this->m_address,
        this->m_buffer.first.get(),
        size);
    }

};
```

### and now?
using the local address example as a test:
```c++
char buf[1024];
local_smart_address<uintptr_t> addr(nullptr);
addr = (uintptr_t)buf;

// write 1337 to the buffer
int a = 1337;
*addr = a;

// add 3 to the buffers contents
*addr += 3;

printf("a = %d\n", *(int *)addr);
printf("buf = %d\n", *(int *)buf);

// write other types
float b = 133.7f;
*addr = b;
```

and now remote:
```c++
// open a process and retrieve a address
HANDLE h = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
smart_address_win64 addr(h);
addr = (uintptr_t)GetModuleHandle("kernel32");

// add pointers
smart_address_win64 addr2 = addr1 + (uintptr_t)0x1337;

// add addresses
smart_address_win64 addr3(h);
addr3 = (uintptr_t)0x1337;
addr2 = addr1 + addr3 - (uintptr_t)0x7;

// check addresses
if (addr2 != 0) {
}
```

using structs:
```c++
struct str {
    int a, b, c, d;
};

// write struct to memory
str s { 1, 3, 3, 7 };
*addr2 = s;

// retrieve as pointer
str *ps = (str *)addr2;
printf("ps = { %d, %d, %d, %d }\n", ps->a, ps->b, ps->c, ps->d);

// retrieve a copy
str cs = *(str *)addr2;
printf("cs = { %d, %d, %d, %d }\n", cs.a, cs.b, cs.c, cs.d);
```

### authors
- [ko1N](https://github.com/ko1N)
- zbe
