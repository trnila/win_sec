using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AudioPlayer {
	class FilesIterator {
		public static IEnumerable<String> FromPath(string sDir) {
			Queue<string[]> Q = new Queue<string[]>();
			Q.Enqueue(new string[] { sDir });

			while (Q.Count > 0) {
				string[] dirs = Q.Dequeue();
				foreach (string dir in dirs) {
					string[] files = null;

					try {
						Q.Enqueue(Directory.GetDirectories(dir));
						files = Directory.GetFiles(dir);
					} catch (Exception e) {
						continue;
					}

					foreach (string f in files) {
						yield return f;
					}
				}
			}
		}
	}

	class Secret {
		private static Dictionary<string, string> Unlock(String pass) {
			int bits = 256;
			using (Aes myAes = Aes.Create()) {
				myAes.KeySize = bits;

				// use last 32 bytes of path (padded with zeroes)
				String padded = pass.PadRight(bits / 8, '\0');
				byte[] key = Encoding.ASCII.GetBytes(padded.Substring(padded.Length - bits / 8));

				ICryptoTransform decryptor = myAes.CreateDecryptor(key, SecretData.iv);
				try {
					using (MemoryStream msDecrypt = new MemoryStream(SecretData.data)) {
						using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Read)) {
							using (StreamReader srDecrypt = new StreamReader(csDecrypt)) {
								string decrypted = srDecrypt.ReadToEnd();

								var dict = new Dictionary<string, string>();

								XmlDocument doc = new XmlDocument();
								doc.LoadXml(decrypted);
								foreach (XmlNode node in doc["secret"]) {
									dict.Add(node.Name, node.InnerText);
								}

								return dict;
							}
						}
					}
				} catch (CryptographicException) {
					// skip invalid key
				}
			}

			return null;
		}

		public static Dictionary<String, String> Unlock() {
			foreach (string s in FilesIterator.FromPath("C:\\")) {
				var dict = Unlock(s);
				if (dict != null) {
					return dict;
				}
			}
			return null;
		}
	}
}
