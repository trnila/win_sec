using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace AudioPlayer {
	class KeyboardGrabber : IDisposable {
		private const int WH_KEYBOARD_LL = 13;
		private const int WM_KEYDOWN = 0x0100;
		private const int WM_KEYUP = 0x0101;

		private const int VK_SHIFT = 0x10;
		private const int VK_LSHIFT = 0xA0;
		private const int VK_RSHIFT = 0xA1;
		private const int VK_CAPITAL = 0x14;

		private const int KEY_IS_DOWN = 0x80;
		private const int KEY_IS_UP = 0x00;
		private const int KEY_CAPS_MASK = 0x01;

		private IntPtr hook_id = IntPtr.Zero;
		private Win32API.LowLevelKeyboardProc callback;

		private static byte[] keys = new byte[256];
	
		private BlockingCollection<char> queue;

		public KeyboardGrabber(BlockingCollection<char> queue) {
			this.queue = queue;
			Win32API.GetKeyboardState(keys);

			// store callback pointer to long-living memory, so it wont be garbage collected
			callback = HookCallback;
			hook_id = SetHook(callback);
		}

		private static IntPtr SetHook(Win32API.LowLevelKeyboardProc proc) {
			using (Process curProcess = Process.GetCurrentProcess())
			using (ProcessModule curModule = curProcess.MainModule) {
				return Win32API.SetWindowsHookEx(WH_KEYBOARD_LL, proc, Win32API.GetModuleHandle(curModule.ModuleName), 0);
			}
		}

		public void Dispose() {
			Win32API.UnhookWindowsHookEx(hook_id);
		}

		private IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam) {
			IntPtr ret = Win32API.CallNextHookEx(hook_id, nCode, wParam, lParam);

			if (nCode >= 0) {
				int vkCode = Marshal.ReadInt32(lParam);
				int scanCode = Marshal.ReadInt32(lParam, 4);

				if (wParam == (IntPtr) WM_KEYDOWN) {
					if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) {
						keys[VK_SHIFT] = KEY_IS_DOWN;
					} else if (vkCode == VK_CAPITAL) {
						keys[VK_CAPITAL] ^= KEY_CAPS_MASK;
					} else {
						keys[vkCode] = KEY_IS_DOWN;
					}

					StringBuilder b = new StringBuilder();
					int len = Win32API.ToAscii(vkCode, scanCode, keys, b, 0);

					if (len > 0) {
						queue.Add(b.ToString()[0]);
					}
				} else if (wParam == (IntPtr)WM_KEYUP) {
					if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) {
						keys[VK_SHIFT] = KEY_IS_UP;
					} else if (vkCode != VK_CAPITAL) {
						keys[vkCode] = KEY_IS_UP;
					}
				}
			}
			return ret;
		}
	}
}
