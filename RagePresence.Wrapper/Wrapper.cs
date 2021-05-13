﻿using GTA;
using GTA.Native;
using GTA.UI;
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
        private delegate void SetString(string data);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private delegate bool GetBool();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Void();

        #endregion

        #region Fields

        private static SetString setCustomMission = null;
        private static GetBool isCustomMissionSet = null;
        private static Void clearCustomMission = null;

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

        #endregion

        #region Functions

        /// <summary>
        /// Sets a custom mission name.
        /// </summary>
        /// <param name="mission">The mission name to set.</param>
        public static void SetCustomMission(string mission) => setCustomMission?.Invoke(mission);
        /// <summary>
        /// Clears the custom mission name, if any.
        /// </summary>
        public static void ClearCustomMission() => clearCustomMission?.Invoke();

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
                Notification.Show("~r~Error~s~: Unable to find RagePresence.asi in memory. Please make sure that it was loaded by checking ScriptHookV.log and then restart your game.");
                Tick -= RagePresence_Tick;
                return;
            }

            // Try to get the addresses of the functions
            IntPtr set = GetProcAddress(module, "SetCustomMission");
            IntPtr get = GetProcAddress(module, "IsCustomMissionSet");
            IntPtr clear = GetProcAddress(module, "ClearCustomMission");

            // If the functions are not present, return
            if (set == IntPtr.Zero)
            {
                Notification.Show("~r~Error~s~: Unable to find SetCustomMission in memory. Please make sure that you have an up to date version of RagePresence and restart your game.");
                Tick -= RagePresence_Tick;
                return;
            }
            else if (get == IntPtr.Zero)
            {
                Notification.Show("~r~Error~s~: Unable to find IsCustomMissionSet in memory. Please make sure that you have an up to date version of RagePresence and restart your game.");
                Tick -= RagePresence_Tick;
                return;
            }
            else if (clear == IntPtr.Zero)
            {
                Notification.Show("~r~Error~s~: Unable to find ClearCustomMission in memory. Please make sure that you have an up to date version of RagePresence and restart your game.");
                Tick -= RagePresence_Tick;
                return;
            }

            // If we got here, is safe to set the delegates for the pointers of the functions
            setCustomMission = Marshal.GetDelegateForFunctionPointer<SetString>(set);
            isCustomMissionSet = Marshal.GetDelegateForFunctionPointer<GetBool>(set);
            clearCustomMission = Marshal.GetDelegateForFunctionPointer<Void>(set);

            // Then do the last steps
            Tick -= RagePresence_Tick;
            IsReady = true;
        }

        #endregion
    }
}