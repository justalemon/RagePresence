#if SHVDN3
using GTA.UI;
#endif
using GTA;
using GTA.Native;
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace RagePresence
{
    /// <summary>
    /// Main Class for interacting with RagePresence.
    /// </summary>
    public class RagePresence : Script
    {
        #region External Imports

        [DllImport("kernel32.dll")]
        private static extern IntPtr GetModuleHandle(string lpFileName);
        [DllImport("kernel32.dll")]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

        #endregion

        #region Delegates

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate IntPtr GetString();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void SetString(string data);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private delegate bool GetBool();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Void();

        #endregion

        #region Fields

        private static SetString setCustomMission = null;
        private static GetString getCustomMission = null;
        private static GetBool isCustomMissionSet = null;
        private static Void clearCustomMission = null;

        private static SetString setCustomDetails = null;
        private static GetString getCustomDetails = null;
        private static GetBool areCustomDetailsSet = null;
        private static Void clearCustomDetails = null;

        private static SetString setCustomState = null;
        private static GetString getCustomState = null;
        private static GetBool isCustomStateSet = null;
        private static Void clearCustomState = null;

        #endregion

        #region Properties

        /// <summary>
        /// Checks if the RagePresence ASI is installed.
        /// </summary>
        public static bool IsInstalled => File.Exists("RagePresence.asi");
        /// <summary>
        /// Checks if the RagePresence wrapper is ready.
        /// </summary>
        public static bool IsReady { get; private set; } = false;

        /// <summary>
        /// Checks if there is a custom mission set.
        /// </summary>
        /// <returns><see langword="true"/> if there is a custom mission set, <see langword="false"/> otherwise.</returns>
        public static bool IsCustomMissionSet => (bool)(isCustomMissionSet?.Invoke());
        /// <summary>
        /// Checks if the details text has been changed manually.
        /// </summary>
        /// <returns><see langword="true"/> if the text was changed, <see langword="false"/> otherwise.</returns>
        public static bool AreCustomDetailsSet => (bool)(areCustomDetailsSet?.Invoke());
        /// <summary>
        /// Checks if the state text has been changed manually.
        /// </summary>
        /// <returns><see langword="true"/> if the text was changed, <see langword="false"/> otherwise.</returns>
        public static bool IsCustomStateSet => (bool)(isCustomStateSet?.Invoke());
        /// <summary>
        /// The name of the current Custom Mission.
        /// Set it to null to clear it.
        /// </summary>
        public static string CustomMission
        {
            get
            {
                if (!IsCustomMissionSet || getCustomMission == null)
                {
                    return null;
                }
                return Marshal.PtrToStringAnsi(getCustomMission.Invoke());
            }
            set
            {
                if (value == null)
                {
                    clearCustomMission?.Invoke();
                }
                else
                {
                    setCustomMission?.Invoke(value);
                }
            }
        }
        /// <summary>
        /// The currently set Custom Details.
        /// Set it to null to clear it.
        /// </summary>
        public static string CustomDetails
        {
            get
            {
                if (!AreCustomDetailsSet || getCustomDetails == null)
                {
                    return null;
                }
                return Marshal.PtrToStringAnsi(getCustomDetails.Invoke());
            }
            set
            {
                if (value == null)
                {
                    clearCustomDetails?.Invoke();
                }
                else
                {
                    setCustomDetails?.Invoke(value);
                }
            }
        }
        /// <summary>
        /// The current Custom State.
        /// Set it to null to clear it.
        /// </summary>
        public static string CustomState
        {
            get
            {
                if (!IsCustomStateSet || getCustomState == null)
                {
                    return null;
                }
                return Marshal.PtrToStringAnsi(getCustomState.Invoke());
            }
            set
            {
                if (value == null)
                {
                    clearCustomState?.Invoke();
                }
                else
                {
                    setCustomState?.Invoke(value);
                }
            }
        }

        #endregion

        #region Constructors

        /// <summary>
        /// Creates a new RaegPresence Wrapper.
        /// </summary>
        /// <remarks>
        /// This should not be created manually.
        /// </remarks>
        public RagePresence()
        {
            Tick += RagePresence_Tick;
        }

        #endregion

        #region Functions

        private void Error(string message)
        {
            string txt = $"~r~Error~s~: {message}";
#if SHVDN2
            UI.Notify(txt);
#elif SHVDN3
            Notification.Show(txt);
#endif
            Tick -= RagePresence_Tick;
        }
        private void PointerError(string name)
        {
            Error($"Unable to find {name} in memory. Please make sure that you have an up to date version of RagePresence and restart your game.");
        }

        #endregion

        #region Ticks

        private void RagePresence_Tick(object sender, EventArgs e)
        {
            // Wait until the game has loaded
            while (Function.Call<bool>(Hash.GET_IS_LOADING_SCREEN_ACTIVE))
            {
                Yield();
            }

            // Get the address of the main RagePresence ASI File
            IntPtr module = GetModuleHandle("RagePresence.asi");

            // If is not there, return
            if (module == IntPtr.Zero)
            {
                Error("Unable to find RagePresence.asi in memory. Please make sure that it was loaded by checking ScriptHookV.log and then restart your game.");
                return;
            }

            // Try to get the addresses of the functions
            IntPtr missionSet = GetProcAddress(module, "SetCustomMission");
            IntPtr missionGet = GetProcAddress(module, "GetCustomMission");
            IntPtr missionCheck = GetProcAddress(module, "IsCustomMissionSet");
            IntPtr missionClear = GetProcAddress(module, "ClearCustomMission");

            IntPtr detailsSet = GetProcAddress(module, "SetCustomDetails");
            IntPtr detailsGet = GetProcAddress(module, "GetCustomDetails");
            IntPtr detailsCheck = GetProcAddress(module, "AreCustomDetailsSet");
            IntPtr detailsClear = GetProcAddress(module, "ClearCustomDetails");

            IntPtr stateSet = GetProcAddress(module, "SetCustomState");
            IntPtr stateGet = GetProcAddress(module, "GetCustomState");
            IntPtr stateCheck = GetProcAddress(module, "IsCustomStateSet");
            IntPtr stateClear = GetProcAddress(module, "ClearCustomState");

            // If the functions are not present, return
            if (missionSet == IntPtr.Zero)
            {
                PointerError("SetCustomMission");
                return;
            }
            if (missionGet == IntPtr.Zero)
            {
                PointerError("GetCustomMission");
                return;
            }
            else if (missionCheck == IntPtr.Zero)
            {
                PointerError("IsCustomMissionSet");
                return;
            }
            else if (missionClear == IntPtr.Zero)
            {
                PointerError("ClearCustomMission");
                return;
            }

            else if (detailsSet == IntPtr.Zero)
            {
                PointerError("SetCustomDetails");
                return;
            }
            else if (detailsGet == IntPtr.Zero)
            {
                PointerError("GetCustomDetails");
                return;
            }
            else if (detailsCheck == IntPtr.Zero)
            {
                PointerError("AreCustomDetailsSet");
                return;
            }
            else if (detailsClear == IntPtr.Zero)
            {
                PointerError("ClearCustomDetails");
                return;
            }

            else if (stateSet == IntPtr.Zero)
            {
                PointerError("SetCustomState");
                return;
            }
            else if (stateGet == IntPtr.Zero)
            {
                PointerError("GetCustomState");
                return;
            }
            else if (stateCheck == IntPtr.Zero)
            {
                PointerError("IsCustomStateSet");
                return;
            }
            else if (stateClear == IntPtr.Zero)
            {
                PointerError("ClearCustomState");
                return;
            }

            // If we got here, is safe to set the delegates for the pointers of the functions
            setCustomMission = Marshal.GetDelegateForFunctionPointer<SetString>(missionSet);
            getCustomMission = Marshal.GetDelegateForFunctionPointer<GetString>(missionGet);
            isCustomMissionSet = Marshal.GetDelegateForFunctionPointer<GetBool>(missionCheck);
            clearCustomMission = Marshal.GetDelegateForFunctionPointer<Void>(missionClear);

            setCustomDetails = Marshal.GetDelegateForFunctionPointer<SetString>(detailsSet);
            getCustomDetails = Marshal.GetDelegateForFunctionPointer<GetString>(detailsGet);
            areCustomDetailsSet = Marshal.GetDelegateForFunctionPointer<GetBool>(detailsCheck);
            clearCustomDetails = Marshal.GetDelegateForFunctionPointer<Void>(detailsClear);

            setCustomState = Marshal.GetDelegateForFunctionPointer<SetString>(stateSet);
            getCustomState = Marshal.GetDelegateForFunctionPointer<GetString>(stateGet);
            isCustomStateSet = Marshal.GetDelegateForFunctionPointer<GetBool>(stateCheck);
            clearCustomState = Marshal.GetDelegateForFunctionPointer<Void>(stateClear);

            // Then do the last steps
            Tick -= RagePresence_Tick;
            IsReady = true;
        }

        #endregion
    }
}
