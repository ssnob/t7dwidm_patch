using t7dwidm_protect.Cheats;
using Refract.UI.Core.Interfaces;
using Refract.UI.Core.Singletons;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.ExThreads;
using System.IO;

namespace t7dwidm_protect
{
    public partial class MainForm : Form, IThemeableControl
    {
        private bool SpendingVials;
        private bool SpendingKeys;
        private Action UpdateProtection;
        public MainForm()
        {
            NativeStealth.SetStealthMode(NativeStealthType.ManualInvoke);
            InitializeComponent();
            UIThemeManager.OnThemeChanged(this, OnThemeChanged_Implementation);
            this.SetThemeAware();
            Text = "Anti-DWIM Crash";
            MaximizeBox = true;
            MinimizeBox = true;
            UpdateProtection = __updateprotection;
            new Task(() => 
            {
                PollProcesses();
            }).Start();
        }

        private void PollProcesses()
        {
            while(true)
            {
                UpdateProtection?.Invoke();
                System.Threading.Thread.Sleep(5000);
            }
        }

        private List<int> pidsOwned = new List<int>();
        private void __updateprotection()
        {
            try
            {
                if (BlackOps3.Game != null)
                {
                    BlackOps3.Game.BaseProcess.Refresh();

                    // check if the game has exited
                    if (BlackOps3.Game.BaseProcess.HasExited)
                    {
                        throw new Exception();
                    }

                    do
                    {
                        // protect a new process if found
                        if (!pidsOwned.Contains(BlackOps3.Game.BaseProcess.Id))
                        {
                            if (BlackOps3.Game.GetValue<long>(BlackOps3.Game[BlackOps3.Constants.OFF_GAME_READY]) == 0)
                            {
                                break;
                            }

                            // disable voice packet dispatching
                            BlackOps3.SetDvar("maxvoicepacketsperframe", "0");

                            var dll = Path.Combine(Environment.CurrentDirectory, "t7dwidm_patch.dll");
                            var hMemory = BlackOps3.Game.QuickAlloc(dll.Length + 1);
                            BlackOps3.Game.SetString(hMemory, dll);
                            long procAddress = (PointerEx)Injector.GetProcAddress(Injector.GetModuleHandle("kernel32.dll"), "LoadLibraryA");
                            BlackOps3.Game.Call<long>(procAddress, hMemory);

                            System.Threading.Thread.Sleep(100);

                            var targetThread = BlackOps3.Game.GetEarliestActiveThread();
                            PointerEx hThread = NativeStealth.OpenThread((int)ThreadAccess.THREAD_HIJACK, false, targetThread.Id);

                            NativeStealth.SuspendThread(hThread);
                            ThreadContext64Ex ctx64 = new ThreadContext64Ex(ThreadContextExFlags.All);

                            if (!ctx64.GetContext(hThread))
                            {
                                NativeStealth.ResumeThread(hThread);
                                break;
                            }

                            ctx64.SetDebug(BlackOps3.Game[BlackOps3.Constants.OFF_DEBUGTARGET], BlackOps3.Game[BlackOps3.Constants.OFF_CL_HandleVoiceTypePacket], 0);
                            ctx64.SetContext(hThread);
                            NativeStealth.ResumeThread(hThread);
                            ProcessEx.CloseHandle(hThread);

                            StatusLabel.Text = "Protected Process with ID: " + BlackOps3.Game.BaseProcess.Id;
                            pidsOwned.Add(BlackOps3.Game.BaseProcess.Id);
                            UIThemeManager.ApplyTheme(Orange());
                        }
                    } while (false);
                }
            }
            catch
            {
                StatusLabel.Text = "No game process found.";
                UIThemeManager.ApplyTheme(UIThemeInfo.Default());
            }
        }

        internal static UIThemeInfo Orange()
        {
            UIThemeInfo theme = new UIThemeInfo();
            theme.BackColor = Color.FromArgb(28, 28, 28);
            theme.TextColor = Color.WhiteSmoke;
            theme.AccentColor = Color.DarkOrange;
            theme.TitleBarColor = Color.FromArgb(36, 36, 36);
            theme.ButtonFlatStyle = FlatStyle.Flat;
            theme.ButtonHoverColor = Color.FromArgb(50, 50, 50);
            theme.LightBackColor = Color.FromArgb(36, 36, 36);
            theme.ButtonActive = Color.DarkOrange;
            theme.TextInactive = Color.Gray;
            return theme;
        }

        public IEnumerable<Control> GetThemedControls()
        {
            yield return InnerForm;
            yield return label1;
            yield return label2;
            yield return label3;
            yield return StatusLabel;
        }

        private void OnThemeChanged_Implementation(UIThemeInfo currentTheme)
        {
        }

        private void RPCExample3_Click(object sender, EventArgs e)
        {
        }

        private void ExampleRPC4_Click(object sender, EventArgs e)
        {
        }

        private void button1_Click(object sender, EventArgs e)
        {
        }
    }
}
