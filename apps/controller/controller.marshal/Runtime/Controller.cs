using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System;

namespace conct
{
	public class Controller : IDisposable
	{
		public event Event<Controller, Command> CommandChanged;

		private IntPtr m_nativeInstance;
		private ControllerNative.CommandChangedDelegate m_managedCallback;
		private IntPtr m_nativeCallback;
		private List<Command> m_commands;

		internal Controller(IntPtr nativeInstance)
		{
			m_nativeInstance = nativeInstance;
			m_commands = new List<Command>();

			m_managedCallback = HandleCallback;
			m_nativeCallback = Marshal.GetFunctionPointerForDelegate<ControllerNative.CommandChangedDelegate>(m_managedCallback);
			ControllerNative.RegisterCommandCallback(m_nativeInstance, m_nativeCallback);
		}

		public void Dispose()
		{
			while(m_commands.Count > 0)
			{
				m_commands[0].Dispose();
			}

			if (m_nativeCallback != IntPtr.Zero)
			{
				ControllerNative.UnregisterCommandCallback(m_nativeInstance, m_nativeCallback);
				m_nativeCallback = IntPtr.Zero;
				m_managedCallback = null;
			}

			m_nativeInstance = IntPtr.Zero;
			m_commands = null;
		}

		public Command GetProperty(DeviceAddress deviceAddress, UInt16 instanceId, string name)
		{
			IntPtr commandHandle = ControllerNative.GetProperty(m_nativeInstance, deviceAddress.NativeInstance, instanceId, name);
			if (commandHandle == null)
			{
				return null;
			}

			Command command = new Command(this, commandHandle, true);
			m_commands.Add(command);
			return command;
		}

		public Command SetProperty(DeviceAddress deviceAddress, UInt16 instanceId, string name, Value value)
		{
			IntPtr commandHandle = ControllerNative.SetProperty(m_nativeInstance, deviceAddress.NativeInstance, instanceId, name, value.NativeInstance);
			if (commandHandle == null)
			{
				return null;
			}

			Command command = new Command(this, commandHandle, false);
			m_commands.Add(command);
			return command;
		}

		public Command CallFunction(DeviceAddress deviceAddress, UInt16 instanceId, string name, Value[] values)
		{
			IntPtr nativeValues = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>() * values.Length);

			IntPtr currentElement = nativeValues;
			for (int i = 0; i < values.Length; i++)
			{
				Marshal.WriteIntPtr(currentElement, values[i].NativeInstance);
				currentElement += Marshal.SizeOf<IntPtr>();
			}

			IntPtr commandHandle = ControllerNative.CallFunction(m_nativeInstance, deviceAddress.NativeInstance, instanceId, name, nativeValues, values.Length);

			Marshal.FreeHGlobal(nativeValues);
			if (commandHandle == null)
			{
				return null;
			}

			Command command = new Command(this, commandHandle, true);
			m_commands.Add(command);
			return command;
		}

		internal void ReleaseCommand(Command command)
		{
			m_commands.Remove(command);
			ControllerNative.ReleaseCommand(m_nativeInstance, command.NativeInstance);
		}

		private void HandleCallback(IntPtr commandHandle)
		{
			if (CommandChanged == null)
			{
				return;
			}

			Command command = m_commands.FirstOrDefault(c => c.NativeInstance == commandHandle);
			if (command != null)
			{
				CommandChanged(this, command);
			}
		}
	}
}