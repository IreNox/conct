using System;

namespace conct
{
	public class Command : IDisposable
	{
		private Controller m_controller;
		private IntPtr m_nativeInstance;
		private bool m_returnsValue;
		private Value m_resultValue;

		internal Command(Controller controller, IntPtr nativeInstance, bool returnsValue)
		{
			m_controller = controller;
			m_nativeInstance = nativeInstance;
			m_returnsValue = returnsValue;
		}

		public void Dispose()
		{
			if (m_nativeInstance != IntPtr.Zero)
			{
				m_controller.ReleaseCommand(this);
				m_nativeInstance = IntPtr.Zero;
			}

			m_controller = null;
		}

		internal IntPtr NativeInstance
		{
			get { return m_nativeInstance; }
		}

		public bool IsBusy
		{
			get { return CommandNative.IsBusy(m_nativeInstance); }
		}

		public bool IsFinish
		{
			get { return CommandNative.IsFinish(m_nativeInstance); }
		}

		public bool IsOk
		{
			get { return CommandNative.IsOk(m_nativeInstance); }
		}

		public bool IsFailure
		{
			get { return CommandNative.IsFailure(m_nativeInstance); }
		}

		public Value ResultValue
		{
			get
			{
				if (!m_returnsValue || IsBusy || IsFailure)
				{
					return null;
				}

				if (m_resultValue == null)
				{
					m_resultValue = new Value(CommandNative.GetValue(m_nativeInstance));
				}

				return m_resultValue;
			}
		}
	}
}