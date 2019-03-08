using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace AudioPlayer {
	class AlternateStream: Stream {
		private const uint GENERIC_READ = 0x80000000;
		private const uint GENERIC_WRITE = 0x40000000;

		private const uint FILE_SHARE_READ = 0x01;
		private const uint FILE_SHARE_WRITE = 0x2;
		private const uint FILE_SHARE_DELETE = 0x4;

		private const uint CREATE_ALWAYS = 0x2;
		private const uint OPEN_ALWAYS = 0x4;

		private IntPtr INVALID_HANDLE_VALUE = (IntPtr)(-1);
		private IntPtr handle;
		
		public override bool CanRead => true;
		public override bool CanSeek => false;
		public override bool CanWrite => true;
		public override long Length => throw new NotImplementedException();
		public override long Position { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

		public AlternateStream(string filename, string stream, bool truncate = false) {
			this.handle = CreateFileW(
				$"{filename}:{stream}",
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
				IntPtr.Zero,
				truncate ? CREATE_ALWAYS : OPEN_ALWAYS,
				0,
				IntPtr.Zero
			);
			if(this.handle == INVALID_HANDLE_VALUE) {
				throw new IOException($"failed to open {filename }:{stream}: {Marshal.GetLastWin32Error()}");
			}
		}

		public new void Dispose() {
			CloseHandle(handle);
		}

		public override void Flush() {}

		public override long Seek(long offset, SeekOrigin origin) {
			throw new NotImplementedException();
		}

		public override void SetLength(long value) {
			throw new NotImplementedException();
		}

		public override int Read(byte[] buffer, int offset, int count) {
			uint r;
			unsafe {
				fixed (byte* p = buffer) {
					bool ret = ReadFile(handle, p + offset, (uint) count, out r, IntPtr.Zero);
					if (!ret) {
						throw new IOException($"Failed to read: {Marshal.GetLastWin32Error()}");
					}
				}
			}
			return (int) r;
		}

		public unsafe override void Write(byte[] buffer, int offset, int count) {
			uint total = 0;
			do {
				uint written;
				unsafe {
					fixed (byte* p = buffer) {
						bool ret = WriteFile(handle, p+offset+total, (uint)(count - total), out written, IntPtr.Zero);
						if(!ret) {
							throw new IOException();
						}
					}
				}
				total += written;
			} while (total < count);
		}

		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern System.IntPtr CreateFileW(
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string lpFileName,
			uint dwDesiredAccess,
			uint dwShareMode,
			[InAttribute()] System.IntPtr lpSecurityAttributes,
			uint dwCreationDisposition,
			uint dwFlagsAndAttributes,
			[InAttribute()] System.IntPtr hTemplateFile
		);

		[DllImport("kernel32.dll", SetLastError = true)]
		private unsafe static extern bool WriteFile(
			IntPtr hFile,
			byte* lpBuffer,
			uint nNumberOfBytesToWrite,
			out uint lpNumberOfBytesWritten,
			[In] IntPtr lpOverlapped
		);

		[DllImport("kernel32.dll", SetLastError = true)]
		private unsafe static extern bool ReadFile(
			IntPtr hFile,
			byte* lpBuffer,
			uint nNumberOfBytesToWrite,
			out uint lpNumberOfBytesWritten,
			[In] IntPtr lpOverlapped
		);

		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern bool CloseHandle(IntPtr handle);
	}
}
