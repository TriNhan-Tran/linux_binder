# Build and Run a Custom WSL2 Kernel with Android Binder Support

This document summarizes the full process to build a custom Linux kernel for **WSL2**, enable **Android Binder IPC**, and successfully run it on Windows.

---

## 1. Environment

* Host OS: **Windows (WSL2)**
* Distro: **Ubuntu on WSL2**
* Kernel version (example): `6.6.x`
* Goal:

  * Enable `CONFIG_ANDROID_BINDER_IPC`
  * Enable `CONFIG_ANDROID_BINDERFS`
  * Use Binder inside WSL2

---

## 2. Required Build Dependencies (Ubuntu / WSL2)

Install all required packages **before** building the kernel:

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    flex bison \
    libncurses-dev \
    pkg-config \
    libssl-dev \
    libelf-dev \
    dwarves \
    elfutils \
    bc \
    cpio \
    rsync \
    kmod
```

> These dependencies cover `menuconfig`, BTF, OpenSSL, kernel headers, and packaging tools.

---

## 3. Kernel Configuration

Run menuconfig:

```bash
make menuconfig
```

Enable the following options:

```
General setup  --->
    [*] Android Binder IPC Driver
    [*] Android Binderfs filesystem
    (binder,hwbinder,vndbinder) Android Binder devices
    [ ] Android Binder IPC Driver Selftest
```

Optional (but recommended):

```
Kernel hacking  --->
    [*] Compile the kernel with BTF debug info
```

Save and exit.

---

## 4. Build the Kernel

```bash
make -j$(nproc)
```

If the build succeeds, the kernel image will be located at:

```text
arch/x86/boot/bzImage
```

---

## 5. Install Custom Kernel for WSL2

### 5.1 Copy Kernel to Windows Filesystem

WSL2 can only load kernels from the **Windows side**.

Example:

```bash
cp arch/x86/boot/bzImage /mnt/c/wsl/bzImage
```

Ensure the file exists in Windows:

```powershell
dir C:\wsl\bzImage
```

---

### 5.2 Configure `.wslconfig`

Create or edit the file:

```
C:\Users\<WINDOWS_USER>\.wslconfig
```

Correct configuration (important: path escaping):

```ini
[wsl2]
kernel=C:/wsl/bzImage
```

> Notes:
>
> * `.wslconfig` **must** be in `C:\Users\<WINDOWS_USER>`
> * Use `/` or double backslashes `\\`
> * A single `\` will cause parsing errors

---

### 5.3 Restart WSL2

From **PowerShell / CMD**:

```powershell
wsl --shutdown
```

Then start WSL again:

```powershell
wsl
```

---

## 6. Verify Custom Kernel Is Running

Inside WSL:

```bash
uname -r
```

Expected:

* ❌ Not `microsoft-standard-WSL2`
* ✔ Shows your custom kernel version (often with `+` suffix)

---

## 7. Enable and Test Binder

### 7.1 Mount BinderFS

```bash
sudo mkdir -p /dev/binderfs
sudo mount -t binder binder /dev/binderfs
```

### 7.2 Verify Binder Devices

```bash
ls /dev/binderfs
```

Expected output:

```text
binder
hwbinder
vndbinder
```

---

## 8. Verify Kernel Configuration (Optional)

```bash
zcat /proc/config.gz | grep BINDER
```

Expected:

```text
CONFIG_ANDROID_BINDER_IPC=y
CONFIG_ANDROID_BINDERFS=y
```

---

## 8. Verify Kernel Configuration (Optional)

```bash
zcat /proc/config.gz | grep BINDER
```

Expected:

```text
CONFIG_ANDROID_BINDER_IPC=y
CONFIG_ANDROID_BINDERFS=y
```

---

## 8.5 Build & Test Binder Applications

After setting up the kernel, build the fixed client/server applications:

```bash
cd /path/to/binder/
gcc -o server server.c -Wall
gcc -o client client.c -Wall
```

Or run the test script:

```bash
./test.sh
```

Expected output:
```
[server] Set as context manager
[server] Entered looper, waiting for transactions...
[server] RX: pid=XXXXX, msg='ping' (size=5)
[server] TX: reply 'pong'

[client] write_consumed: XX, read_consumed: XX
[client] transaction complete
[client] got reply: pong (size=5)
```

---

## 9. Important Bug Fixes Applied

See [FIXES.md](FIXES.md) for detailed explanation of:

1. **MAP_SHARED vs MAP_PRIVATE** - Critical for kernel-userspace memory sharing
2. **Data location** - Messages must be in shared buffer, not stack
3. **Reply transaction handling** - Proper target addressing
4. **Resource cleanup** - Complete error handling and resource deallocation
5. **Command parsing** - Correct handling of multiple BR commands

---

## 10. Common Pitfalls

* Kernel builds successfully but Binder is missing

  * → Custom kernel is **not** loaded by WSL2
* `Invalid escaped character` error

  * → Wrong path escaping in `.wslconfig`
* `openssl/bio.h` not found

  * → Missing `libssl-dev`
* `cpio could not be found`

  * → Missing `cpio` package

---

## 11. Previous Issues Resolved

The original code was encountering kernel errors:
```
kernel: binder_alloc: 31959: binder_alloc_buf, no vma
kernel: binder: cannot allocate buffer: vma cleared, target dead or dying
kernel: binder: 32404:32404 transaction call to 31959:0 failed 18/29189/-3
```

**Root causes fixed:**
1. Changed `MAP_PRIVATE | MAP_ANONYMOUS` → `MAP_SHARED` for proper kernel-userspace memory sharing
2. Moved data pointers from stack to shared buffer
3. Fixed reply transaction targeting
4. Added proper resource cleanup on all error paths
5. Implemented correct BR command parsing loop

See [FIXES.md](FIXES.md) for detailed technical explanations.

---

## 12. Next Steps

---

## Status

✅ Custom WSL2 kernel built
✅ Binder IPC enabled
✅ BinderFS mounted and working

You now have a fully functional **Android Binder–enabled WSL2 kernel**.
