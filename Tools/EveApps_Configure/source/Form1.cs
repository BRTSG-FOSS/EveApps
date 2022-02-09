using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace EveAppsConfig
{
    public partial class Form1 : Form
    {
        string[] module =
        {
                "",
                "VM800B35A_BK        ",
                "VM800B43A_BK        ",
                "VM800B50A_BK        ",
                "VM801B43A_BK        ",
                "VM801B50A_BK        ",
                "VM800C35A_N         ",
                "VM800C43A_N         ",
                "VM800C35A_D         ",
                "VM800C43A_D         ",
                "VM800C50A_D         ",
                "ME812A_WH50R        ",
                "ME812AU_WH50R       ",
                "ME813A_WH50C        ",
                "ME813AU_WH50C       ",
                "VM810C50A_D         ",
                "VM810C50A_N         ",
                "ME810A_HV35R        ",
                "VM816C50A_D         ",
                "VM816C50A_N         ",
                "VM816CU50A_D        ",
                "VM816CU50A_N        ",
                "ME810AU_HV35R       ",
                "ME813A_WV7C         ",
                "PANL35              ",
                "PANL50              ",
                "PANL70              ",
                "PANL70PLUS          ",
                "EVE_GRAPHICS_VM800C ",
                "EVE_GRAPHICS_VM810C ",
                "EVE_GRAPHICS_VM816C ",
                "EVE_GRAPHICS_ME817EV",
                "EVE_GRAPHICS_GD3X_DAZZLER",
        };

        string[] EveIC = {
                "",
                "EVE_GRAPHICS_FT800",
                "EVE_GRAPHICS_FT801",
                "EVE_GRAPHICS_FT810",
                "EVE_GRAPHICS_FT811",
                "EVE_GRAPHICS_FT812",
                "EVE_GRAPHICS_FT813",
                "EVE_GRAPHICS_BT815",
                "EVE_GRAPHICS_BT816",
                "EVE_GRAPHICS_BT817",
                "EVE_GRAPHICS_BT818"
        };

        string[] Ft900host =
        {
                "   ",
                "MM900EV1A               ",
                "MM900EV1B               ",
                "MM900EV2A               ",
                "MM900EV3A               ",
                "MM900EV_LITE            ",
        };

        string[] Ft930host =
        {
                "   ",
                "MM930MINI               ",
                "MM930LITE               ",
                "MM932LC                 ",
        };
        string[] MsvcHost =
        {
                "   ",
                // "BT8XXEMU_PLATFORM   ", Emulator have only 1 mode, non configurable
                "EVE_PLATFORM_FT4222     ",
                "EVE_PLATFORM_MPSSE      ",
                "EVE_PLATFORM_RP2040     ",
        };
        string[] LCD =
        {
                "                                        ",
                "EVE_DISPLAY_QVGA                        ",
                "EVE_DISPLAY_WQVGA                       ",
                "EVE_DISPLAY_WVGA                        ",
                "EVE_DISPLAY_WSVGA                       ",
                "EVE_DISPLAY_WXGA                        ",
                //"EVE_DISPLAY_AT043B35                    ",
                "EVE_DISPLAY_ILI9488_HVGA_PORTRAIT       ",
                "EVE_DISPLAY_KD2401_HVGA_PORTRAIT        "
        };
        string[] LCD_D=
        { 
                "                                            ",
                "EVE_DISPLAY_QVGA (320x240)                  ",
                "EVE_DISPLAY_WQVGA (480x272)                 ",
                "EVE_DISPLAY_WVGA (800x480)                  ",
                "EVE_DISPLAY_WSVGA (1024x600)                ",
                "EVE_DISPLAY_WXGA (1280x800)                 ",
                //"EVE_DISPLAY_AT043B35 (480x272)              ",
                "EVE_DISPLAY_ILI9488_HVGA_PORTRAIT (320x480) ",
                "EVE_DISPLAY_KD2401_HVGA_PORTRAIT (320x480)  "
        };
        string[] Flash =
        {
                "          ",
                "EVE_FLASH_W25Q16          ",
                "EVE_FLASH_W25Q32          ",
                "EVE_FLASH_W25Q64          ",
                "EVE_FLASH_W25Q128         ",
                "EVE_FLASH_MX25L16         ",
                "EVE_FLASH_MX25L32         ",
                "EVE_FLASH_MX25L64         ",
                "EVE_FLASH_MX25L128        ",
                "EVE_FLASH_MX25L256        ",
                "EVE_FLASH_MX25L512        ",
                "EVE_FLASH_MX25L1024       ",
                "EVE_FLASH_MX25L2048       ",
            };
        string[] Support =
        {
                "       ",
                "EVE_SUPPORT_FLASH       ",
                "EVE_SUPPORT_UNICODE     ",
                "EVE_SUPPORT_ASTC        ",
                "EVE_SUPPORT_PNG         ",
                "EVE_SUPPORT_VIDEO       ",
                "EVE_SUPPORT_CMDB        ",
                "EVE_SUPPORT_MEDIAFIFO   ",
                "EVE_SUPPORT_CAPACITIVE  ",
                "EVE_SUPPORT_RESISTIVE   ",
            };
        string[] Demo =
        {
                "All                                 ",
                "DemoApps/AudioPlayback              ",
                "DemoApps/CircleView                 ",
                "DemoApps/EvChargePoint              ",
                "DemoApps/FlashBitbang               ",
                "DemoApps/Gauges                     ",
                "DemoApps/Gradient                   ",
                "DemoApps/Graph                      ",
                "DemoApps/HDPictureViewer            ",
                "DemoApps/Imageviewer                ",
                "DemoApps/Imageviewer2               ",
                "DemoApps/Instrument                 ",
                "DemoApps/Jackpot                    ",
                "DemoApps/Keyboard                   ",
                "DemoApps/Lift                       ",
                "DemoApps/Lift2                      ",
                "DemoApps/Mainmenu                   ",
                "DemoApps/MediaPlayer                ",
                "DemoApps/Metaballs                  ",
                "DemoApps/Meter                      ",
                "DemoApps/Refrigerator               ",
                "DemoApps/RotaryDial                 ",
                "DemoApps/RunningBar                 ",
                "DemoApps/Signals                    ",
                "DemoApps/Signature                  ",
                "DemoApps/Sketch                     ",
                "DemoApps/Unicode                    ",
                "DemoApps/UnicodeRuntime             ",
                "DemoApps/WashingMachine             ",
                "SampleApp/Widget                    ",
                "SampleApp/Video                     ",
                "SampleApp/Utility                   ",
                "SampleApp/Touch                     ",
                "SampleApp/Sound                     ",
                "SampleApp/Primitives                ",
                "SampleApp/Power                     ",
                "SampleApp/Font                      ",
                "SampleApp/Flash                     ",
                "SampleApp/Bitmap                    ",
                "SampleApp/Animation                 ",
            };

        string[] Touch =
        {
                "                               ",
                "EVE_TOUCH_FOCAL                ",
                "EVE_TOUCH_GOODIX               ",
                "EVE_TOUCH_RESISTIVE            ",
                "EVE_TOUCH_DISABLED             ",
        };

        string[] ToBeDelete = // This should be deleted from user macro
        {
                "BT8XXEMU_PLATFORM               ", 
                "FT9XX_PLATFORM                  ",           
                "FT4222_PLATFORM                 ",          
                "MPSSE_PLATFORM                  ",           
                "RP2040_PLATFORM                 ",          
                "DISPLAY_RESOLUTION_             ",      
                "DISPLAY_RESOLUTION_QVGA         ",  
                "DISPLAY_RESOLUTION_WQVGA        ", 
                "DISPLAY_RESOLUTION_WVGA         ",  
                "DISPLAY_RESOLUTION_WSVGA        ", 
                "DISPLAY_RESOLUTION_WXGA         ",  
                "DISPLAY_RESOLUTION_HVGA_PORTRAIT", 
                " ", 
        };

        private void initDefault()
        {
            foreach (String i in module)
                cbb_Module.Items.Add(i);
            foreach (String i in EveIC)
                cbb_EveIC.Items.Add(i);
            foreach (String i in MsvcHost)
                cbb_msvchost.Items.Add(i);
            foreach (String i in Ft900host)
                cbb_ft900.Items.Add(i);
            foreach (String i in Ft930host)
                cbb_ft930.Items.Add(i);
            foreach (String i in LCD_D)
                cbb_LCD.Items.Add(i);
            foreach (String i in Flash)
                cbb_Flash.Items.Add(i);
            foreach (String i in Demo)
                cbb_Demo.Items.Add(i);
            foreach (String i in Touch)
                cbb_Touch.Items.Add(i);

            cbb_Module.SelectedIndex = 0;
            cbb_EveIC.SelectedIndex = 0;
            cbb_msvchost.SelectedIndex = 0;
            cbb_ft900.SelectedIndex = 0;
            cbb_ft930.SelectedIndex = 0;
            cbb_LCD.SelectedIndex = 0;
            cbb_Flash.SelectedIndex = 0;
            cbb_Demo.SelectedIndex = 0;
            cbb_Touch.SelectedIndex = 0;

            txt_root.Text = "../../../EveApps";
        }
        public Form1()
        {
            InitializeComponent();
            initDefault();
            this.restoreDefault(false);
        }

        public static string PrintXML(string xml, bool isEclipse)
        {
            string result = "";

            MemoryStream mStream = new MemoryStream();
            XmlTextWriter writer = new XmlTextWriter(mStream, Encoding.Unicode);
            XmlDocument document = new XmlDocument();

            if (isEclipse)
            {
                writer.Indentation = 1;
                writer.IndentChar = '\t';
            }

            try
            {
                // Load the XmlDocument with the XML.
                document.LoadXml(xml);

                writer.Formatting = Formatting.Indented;

                // Write the XML into a formatting XmlTextWriter
                document.WriteContentTo(writer);
                writer.Flush();
                mStream.Flush();

                // Have to rewind the MemoryStream in order to read
                // its contents.
                mStream.Position = 0;

                // Read MemoryStream contents into a StreamReader.
                StreamReader sReader = new StreamReader(mStream);

                // Extract the text from the StreamReader.
                string formattedXml = sReader.ReadToEnd();

                if (isEclipse)
                {
                    result = formattedXml.Replace(" />", "/>");
                }
                else
                {
                    result = formattedXml;
                }
            }
            catch (XmlException)
            {
                // Handle the exception
            }

            mStream.Close();
            writer.Close();

            return result;
        }

        private List<XmlNode> findXmlNodeByAttr(XmlDocument xml,
            string tag, string attr, string value)
        {
            List<XmlNode> ret = new List<XmlNode>();

            var nodes = xml.GetElementsByTagName(tag);

            foreach (XmlNode node in nodes)
            {
                if (node.Attributes != null &&
                    node.Attributes[attr] != null &&
                    node.Attributes[attr].Value == value)
                {
                    ret.Add(node);
                }
            }

            return ret;
        }

        private string ft9xReplaceNode(string[] config, string haystack, string needle)
        {
            XmlDocument xml = new XmlDocument();
            xml.LoadXml(haystack);

            Console.WriteLine("Checking needle: " + needle);
            // delete all config
            foreach (String k in config)
            {
                string j = k.Trim();
                if (j == "") continue;
                string tag = "listOptionValue", attr = "value", value = j + "=1";
                XmlNodeList nodes = xml.GetElementsByTagName(tag);

                for (int i = 0; i < nodes.Count; i++)
                {
                    XmlNode node = nodes[i];
                    if (node.Attributes != null &&
                        node.Attributes[attr] != null &&
                        node.Attributes[attr].Value == value)
                    {
                        Console.WriteLine("Deleting " + value);
                        nodes[i].ParentNode.RemoveChild(node);
                    }
                }
            }

            if (needle != "")
            {
                // append needle to haystack
                string nodetxt = "<listOptionValue builtIn=\"false\" value=\"" + needle + "=1\"/>";
                Console.WriteLine("Append to Ft9xx: " + nodetxt);
                return xml.InnerXml.Replace("</option>", nodetxt + "</option>");
            }
            return xml.InnerXml;
        }
        private void setFT9X(string[] filePaths, string[] config, string module_,
            string eveIC_, string host_, string lcd_, string flash_, List<String> support_, string touch_)
        {
            foreach (string f in filePaths)
            {
                Console.WriteLine("Processing " + f + "\r\n");
                Txt_Log.AppendText("Processing " + f + "\r\n");
                Txt_Log.ScrollToCaret();

                string readText = "";
                if (File.Exists(f))
                {
                    readText = File.ReadAllText(f, Encoding.UTF8);
                }
                else
                {
                    continue;
                }

                XmlDocument xml = new XmlDocument();
                xml.LoadXml(readText);

                string tag = "option", attr = "name", value = "Defined symbols (-D)";
                var nodes = xml.GetElementsByTagName(tag);

                for (int i = 0; i < nodes.Count; i++)
                {
                    XmlNode node = nodes[i];
                    if (node.Attributes != null &&
                        node.Attributes[attr] != null &&
                        node.Attributes[attr].Value == value)
                    {
                        string text = node.OuterXml;
                        Console.WriteLine("Found:" + text);

                        text = ft9xReplaceNode(ToBeDelete, text, "");
                        text = ft9xReplaceNode(module, text, module_);
                        text = ft9xReplaceNode(EveIC, text, eveIC_);
                        text = ft9xReplaceNode(config, text, host_);
                        text = ft9xReplaceNode(LCD, text, lcd_);
                        text = ft9xReplaceNode(Flash, text, flash_);
                        text = ft9xReplaceNode(Touch, text, touch_);

                        // Clean support list
                        text = ft9xReplaceNode(Support, text, "");

                        // Append new support list
                        foreach (String s in support_)
                        {
                            text = Ft9xAppendNode(text, s);
                        }

                        node.InnerXml = text;
                        node.InnerXml = node.ChildNodes[0].InnerXml;
                    }
                }

                File.WriteAllText(f, PrintXML(xml.OuterXml, true /*Eclipse*/));
            }
        }

        private string Ft9xAppendNode(string tag, string node)
        {
            // append needle to haystack
            string nodetxt = "<listOptionValue builtIn=\"false\" value=\"" + node + "=1\"/>";
            Console.WriteLine("Append to Ft9xx: " + nodetxt);
            return tag.Replace("</option>", nodetxt + "</option>");
        }

        private string msvcAppendNode(string tag, string node)
        {
            String ret = tag + ";" + node;
            return ret.Replace(";;", ";");
        }

        private string msvcReplaceNode(string[] config, string haystack, string needle)
        {
            foreach (String j in config)
            {
                String i = j.Trim();
                if (i == "") continue;
                haystack = haystack.Replace(i, "");
            }
            if (needle != "") // delete this config
            {
                haystack += ";" + needle;
            }
            return haystack.Replace(";;", ";");
        }

        private void setMSVC(string[] filePaths, string module_,
            string eveIC_, string host_, string lcd_, string flash_, List<String> support_, string touch_)
        {
            const string tag = "PreprocessorDefinitions";

            foreach (string f in filePaths)
            {
                Console.WriteLine("Processing " + f + "\r\n");
                Txt_Log.AppendText("Processing " + f + "\r\n");
                Txt_Log.ScrollToCaret();

                string readText = "";
                if (File.Exists(f))
                {
                    readText = File.ReadAllText(f, Encoding.UTF8);
                }
                else
                {
                    continue;
                }

                XmlDocument xml = new XmlDocument();
                xml.LoadXml(readText);
                XmlNodeList xnList = xml.GetElementsByTagName(tag);
                foreach (XmlNode node in xnList)
                {
                    string text = node.InnerText;
                    Console.WriteLine("Found:" + text);

                    // Delete unwanted macro
                    text = msvcReplaceNode(ToBeDelete, text, "");

                    text = msvcReplaceNode(module, text, module_);
                    text = msvcReplaceNode(EveIC, text, eveIC_);

                    // If not emulator file, change MSVC to FT4222 or MPSSE
                    if (f.Contains("BT8XXEMU_PLATFORM") || f.Contains("Emulator"))
                    {
                        text = msvcReplaceNode(MsvcHost, text, "EVE_PLATFORM_BT8XXEMU");
                    }
                    else
                    {
                        text = msvcReplaceNode(MsvcHost, text, host_);
                    }
                    text = msvcReplaceNode(LCD, text, lcd_);
                    text = msvcReplaceNode(Flash, text, flash_);
                    text = msvcReplaceNode(Touch, text, touch_);

                    // Clean support list
                    text = msvcReplaceNode(Support, text, "");

                    // Append new support list
                    foreach (String i in support_)
                    {
                        text = msvcAppendNode(text, i);
                    }
                    node.InnerText = text;
                }

                File.WriteAllText(f, PrintXML(xml.OuterXml, false /*Msvc*/));
            }
        }
        private string[] seachProjectfile(string demo, string ext)
        {
            string searchPath = txt_root.Text;

            if (!Directory.Exists(searchPath))
            {
                Txt_Log.AppendText("Directory does not exist: " + searchPath + "\r\n");
                return new string[] { };
            }

            if (demo == "") // Seach all folder in this directory
            {
                // Only process demo* and SampleApp folder
                List<string> ret = new List<string>();
                for (int i = 1; i < Demo.Length; i++)
                {
                    string demo_path = searchPath + "\\" + Demo[i].Trim();
                    if (!Directory.Exists(demo_path))
                    {
                        Txt_Log.AppendText("Demo does not exist - " + Demo[i].Trim() + "\r\n");
                        continue;
                    }
                    string[] files = Directory.GetFiles(demo_path, ext, SearchOption.AllDirectories);
                    foreach (String s in files)
                    {
                        ret.Add(s);
                    }
                }
                return ret.ToArray();
            }
            else // Seach for a demo folder in this directory
            {
                string demo_path = searchPath + "\\" + demo;
                if (!Directory.Exists(demo_path))
                {
                    Txt_Log.AppendText("Directory does not exist: " + demo_path + "\r\n");
                    return new string[] { };
                }
                return Directory.GetFiles(demo_path, ext, SearchOption.AllDirectories);
            }

            return new string[] { };
        }

        private void btn_Generate_Click(object sender, EventArgs e)
        {
            string module = "";
            string eveIC = "";
            string mhost = "";
            string f900host = "";
            string f930host = "";
            string lcd = "";
            string flash = "";
            string demo = "";
            string touch = "";
            List<String> support = new List<string>(); // Support list is multiple

            Txt_Log.Clear();

            /// Get UI value
            try { if (cbb_Module.SelectedIndex > -1) { module = cbb_Module.SelectedItem.ToString().Trim(); } } catch { }
            try { if (cbb_EveIC.SelectedIndex > -1) { eveIC = cbb_EveIC.SelectedItem.ToString().Trim(); } } catch { }
            try { if (cbb_msvchost.SelectedIndex > -1) { mhost = cbb_msvchost.SelectedItem.ToString().Trim(); } } catch { }
            try { if (cbb_ft900.SelectedIndex > -1) { f900host = cbb_ft900.SelectedItem.ToString().Trim(); } } catch { }
            try { if (cbb_ft900.SelectedIndex > -1) { f930host = cbb_ft930.SelectedItem.ToString().Trim(); } } catch { }
            try { if (cbb_Flash.SelectedIndex > -1) { flash = cbb_Flash.SelectedItem.ToString().Trim(); } } catch { }
            try { if (cbb_Demo.SelectedIndex > -1) { demo = cbb_Demo.SelectedItem.ToString().Trim(); } } catch { }
            try { if (cbb_Touch.SelectedIndex > -1) { touch = cbb_Touch.SelectedItem.ToString().Trim(); } } catch { }

            try { 
                if (cbb_LCD.SelectedIndex > -1) {
                    int id = cbb_LCD.SelectedIndex;
                    lcd = LCD[id].Trim(); 
                } 
            } catch { }

            // Support list
            if (Cb_Astc.Checked) support.Add(Cb_Astc.Text);
            if (Cb_Capacitive.Checked) support.Add(Cb_Capacitive.Text);
            if (Cb_Cmdb.Checked) support.Add(Cb_Cmdb.Text);
            if (Cb_Flash.Checked) support.Add(Cb_Flash.Text);
            if (Cb_Mediafifo.Checked) support.Add(Cb_Mediafifo.Text);
            if (Cb_Png.Checked) support.Add(Cb_Png.Text);
            if (Cb_Resistive.Checked) support.Add(Cb_Resistive.Text);
            if (Cb_Unicode.Checked) support.Add(Cb_Unicode.Text);
            if (Cb_Video.Checked) support.Add(Cb_Video.Text);

            this.do_setup(module, eveIC, mhost, f900host, f930host, lcd, flash, demo, support, touch);
        }

        private void do_setup(string module, string eveIC, string mhost,
            string f900host, string f930host, string lcd, string flash, string demo, 
            List<String> support, string touch)
        {
            if (demo == "All") demo = "";

            // Search MSVC project files
            Txt_Log.AppendText("Searching for *.vcxproj\r\n");
            string[] msvcProj = seachProjectfile(demo, "*.vcxproj");

            // Search and detecf FT900/FT930 project files
            Txt_Log.AppendText("Searching for .cproject\r\n");
            string[] ft9xxProj = seachProjectfile(demo, ".cproject");
            List<string> ft900ProjList = new List<string>();
            List<string> ft930ProjList = new List<string>();
            foreach (string f in ft9xxProj)
            {
                string readText = File.ReadAllText(f, Encoding.UTF8);
                if (readText.Contains("__FT930__"))
                {
                    ft930ProjList.Add(f);
                }
                else
                {
                    ft900ProjList.Add(f);
                }
            }
            String[] ft900Proj = ft900ProjList.ToArray();
            String[] ft930Proj = ft930ProjList.ToArray();

            // Process MSVC project files
            if (msvcProj != null && msvcProj.Length > 0)
            {
                setMSVC(msvcProj, module, eveIC, mhost, lcd, flash, support, touch);
            }

            // Process Ft900 project files
            if (ft900Proj != null && ft900Proj.Length > 0)
            {
                setFT9X(ft900Proj, Ft900host, module, eveIC, f900host, lcd, flash, support, touch);
            }

            // Process Ft930 project files
            if (ft930Proj != null && ft930Proj.Length > 0)
            {
                setFT9X(ft930Proj, Ft930host, module, eveIC, f930host, lcd, flash, support, touch);
            }

            Txt_Log.AppendText("Done\r\n");
        }

        private void btn_browse_Click(object sender, EventArgs e)
        {
            var dialog = new FolderSelectDialog
            {
                InitialDirectory = "",
                Title = "Select EveApps folder"
            };
            if (dialog.Show(Handle))
            {
                Console.WriteLine(dialog.FileName);
                txt_root.Text = dialog.FileName;
            }
        }

        private void restoreDefault(bool isSetup = false)
        {
            Txt_Log.Clear();

            string module = "EVE_GRAPHICS_ME817EV";
            string eveIC = "";
            string mhost = "EVE_PLATFORM_FT4222";
            string f900host = "MM900EV1B";
            string f930host = "MM930LITE";
            string lcd = "EVE_DISPLAY_WVGA";
            string flash = "";
            string demo = "";
            string touch = "";
            List<String> support = new List<string>(); // Support list is multiple

            try { if (cbb_Demo.SelectedIndex > -1) { demo = cbb_Demo.SelectedItem.ToString().Trim(); } } catch { }

            // Special reset for DemoEvChargePoint            
            if (demo == "Demo/EvChargePoint")
            {
                lcd = "EVE_DISPLAY_WXGA";
            }

            cbb_Module.SelectedIndex = cbb_Module.FindString(module);
            cbb_EveIC.SelectedIndex = cbb_EveIC.FindString(eveIC);
            cbb_msvchost.SelectedIndex = cbb_msvchost.FindString(mhost);
            cbb_ft900.SelectedIndex = cbb_ft900.FindString(f900host);
            cbb_ft930.SelectedIndex = cbb_ft930.FindString(f930host);
            cbb_LCD.SelectedIndex = cbb_LCD.FindString(lcd);
            cbb_Flash.SelectedIndex = cbb_Flash.FindString(flash);
            cbb_Touch.SelectedIndex = cbb_Touch.FindString(touch);

            Cb_Astc.Checked = false;
            Cb_Capacitive.Checked = false;
            Cb_Cmdb.Checked = false;
            Cb_Flash.Checked = false;
            Cb_Mediafifo.Checked = false;
            Cb_Png.Checked = false;
            Cb_Resistive.Checked = false;
            Cb_Unicode.Checked = false;
            Cb_Video.Checked = false;

            if (!isSetup)
            {
                return;
            }

            this.do_setup(module, eveIC, mhost, f900host, f930host, lcd, flash, demo, support, touch);

            // Special reset for DemoEvChargePoint        
            if (demo == "" || demo == "All")
            {
                Txt_Log.AppendText("Setting LCD as EVE_DISPLAY_WXGA for DemoEvChargePoint...\r\n");
                demo = "Demo/EvChargePoint";
                lcd = "EVE_DISPLAY_WXGA";
                this.do_setup(module, eveIC, mhost, f900host, f930host, lcd, flash, demo, support, touch);
            }
        }

        private void btn_restoreDefault_Click(object sender, EventArgs e)
        {
            this.restoreDefault(true);
        }

        private void btn_about_Click(object sender, EventArgs e)
        {
            AboutBox1 testDialog = new AboutBox1();

            if (testDialog.ShowDialog(this) == DialogResult.OK)
            {
                // Read the contents of testDialog's TextBox.
            }
            else
            {
            }
            testDialog.Dispose();
        }
    }
}
