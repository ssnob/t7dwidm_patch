using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static t7dwidm_protect.Cheats.BlackOps3.Constants;
using static System.ExCallThreadType;

namespace t7dwidm_protect.Cheats
{
    internal static class BlackOps3
    {
        internal static class Constants
        {
            public const ulong OFF_DEBUGTARGET = 0x143A606;
            public const ulong OFF_GAME_READY = 0x168EF8C8;
            public const ulong OFF_DVAR_SETFROMSTRINGBYNAME = 0x22C7F60;
            public const ulong OFF_CL_HandleVoiceTypePacket = 0x1359310;
        }


        private static ProcessEx __game;
        internal static ProcessEx Game
        {
            get
            {
                if (__game is null || __game.BaseProcess.HasExited)
                {
                    __game = "blackops3";
                    if (__game is null || __game.BaseProcess.HasExited)
                    {
                        throw new Exception("Black Ops 3 process not found.");
                    }
                    __game.SetDefaultCallType(XCTT_RIPHijack);
                }
                if (!__game.Handle)
                {
                    __game.OpenHandle(ProcessEx.PROCESS_ACCESS, true);
                }
                return __game;
            }
            set
            {
                __game = value;
                if (__game is null) return;
                __game.SetDefaultCallType(XCTT_RIPHijack);
                if (!__game.Handle)
                {
                    __game.OpenHandle(ProcessEx.PROCESS_ACCESS, true);
                }
            }
        }

        public static void SetDvar(string name, string value)
        {
            if (name is null || value is null) return;
            var f = Game[OFF_DVAR_SETFROMSTRINGBYNAME];
            Game.Call<long>(f, name, value, true);
        }

        #region typedef
        #endregion
    }
}
