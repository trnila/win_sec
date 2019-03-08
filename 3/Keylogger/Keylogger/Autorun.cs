using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.ServiceProcess;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration.Install;
using System.ComponentModel;
using System.Threading;

namespace AudioPlayer {
	interface IAutoRunner {
		void Install(string executable_path);
	}

	class RegistryAutoRunner: IAutoRunner {
		private String key;
		private String item;

		public RegistryAutoRunner(String key, String item) {
			this.key = key;
			this.item = item;
		}

		//private String key = @"HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run";
		//private String item = "test";

		private bool ShouldAutoRun() {
			string current = (string)Registry.GetValue(key, item, null);
			return current == null || !File.Exists(current);
		}

		public void Install(string executable_path) {
			if (ShouldAutoRun()) {
				Registry.SetValue(key, item, executable_path);
			}
		}
	}

	class AppDataStartupRunner: IAutoRunner {
		public void Install(string executable_path) {
			String dst = Path.Combine(
				Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
				@"Microsoft\Windows\Start Menu\Programs\Startup\audio.exe"
			);

			try {
				File.Copy(executable_path, dst, true);
			} catch(Exception e) {
				Console.WriteLine(e);
			}
		}
	}

	class Service: ServiceBase {
		public Service() {
			this.ServiceName = "Audio";
		}
	}

	[RunInstaller(true)]
	public class MyWindowsServiceInstaller : Installer {
		public MyWindowsServiceInstaller() {
			var processInstaller = new ServiceProcessInstaller();
			var serviceInstaller = new ServiceInstaller();

			processInstaller.Account = ServiceAccount.LocalSystem;

			serviceInstaller.DisplayName = "Audio";
			serviceInstaller.StartType = ServiceStartMode.Automatic;

			serviceInstaller.ServiceName = "Audio";
			this.Installers.Add(processInstaller);
			this.Installers.Add(serviceInstaller);
		}
	}

	class ServiceRunner: IAutoRunner {
		public void Install(string executable_path) {
			if (System.Environment.UserInteractive) {
				ManagedInstallerClass.InstallHelper(new string[] { executable_path });
			} else {
				new Thread(() => ServiceBase.Run(new Service())).Start();
			}
		}
	}
}
