import SwiftUI
import AVFoundation

enum Colorings: String, CaseIterable {
    case darkest = "Darkest"
    case dark = "Dark"
    case neutral = "Neutral"
    case bright = "Bright"
    case brightest  = "Brightest"
}

enum Pitches: String, CaseIterable {
    case lowest = "Lowest"
    case lower = "Lower"
    case neutral = "Neutral"
    case higher = "Higher"
    case highest  = "Highest"
}

@main
struct KeyBrightApp: App {
    @AppStorage("soundFileURL") private var soundFileURL: URL?
    @AppStorage("coloring") private var coloring: String = Colorings.neutral.rawValue
    @AppStorage("pitch") private var pitch: String = Pitches.neutral.rawValue
    
    @State private var audioPlayers: [AVAudioPlayer] = []
    @State private var permissionEnabled: Bool = false
    @State private var monitor: Any?
    
    init() {
        self.permissionEnabled = checkPermission()
        if permissionEnabled {
            self.monitor = startMonitor()
        }
    }
    
    var body: some Scene {
        MenuBarExtra("KeyBright", systemImage: "keyboard") {
            if permissionEnabled {
                HStack {
                    Text(monitor != nil ? "On" : "Paused")
                        .foregroundStyle(monitor == nil ? .secondary : Color.green)
                    Button(monitor != nil ? "Stop" : "Start") {
                        if let m = monitor {
                            stopMonitor(event: m)
                            self.monitor = nil
                        } else {
                            self.monitor = startMonitor()
                        }
                    }
                    .foregroundStyle(monitor == nil ? .secondary : Color.green)
                }
            } else {
                Button("Allow Permission") {
                    self.permissionEnabled = checkPermission()
                    if permissionEnabled {
                        self.monitor = startMonitor()
                    }
                }
            }
            
            Divider()
            
            Menu("Sound") {
                Text(soundFileURL?.lastPathComponent ?? "(Not Set)")
                    .foregroundStyle(.secondary)
                Button("Import...") {
                    let panel = NSOpenPanel()
                    panel.canChooseFiles = true
                    panel.canChooseDirectories = false
                    panel.allowsMultipleSelection = false
                    if panel.runModal() == .OK {
                        if let url = panel.urls.first {
                            self.soundFileURL = url
                        }
                    }
                }
            }
            Picker("Color", selection: $coloring) {
                ForEach(Colorings.allCases, id: \.self) { coloring in
                    Text(coloring.rawValue).tag(coloring.rawValue)
                }
            }
            .pickerStyle(.menu)
            Picker("Pitch", selection: $pitch) {
                ForEach(Pitches.allCases, id: \.self) { pitch in
                    Text(pitch.rawValue).tag(pitch.rawValue)
                }
            }
            .pickerStyle(.menu)
            
            Divider()
            
            Button("Quit") {
                NSApplication.shared.terminate(nil)
            }
            .keyboardShortcut("q")
        }
        .menuBarExtraStyle(.menu)
    }

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        self.permissionEnabled = checkPermission()
        if permissionEnabled {
            self.monitor = startMonitor()
        }
    }
    
    func applicationWillTerminate(_ aNotification: Notification) {
        audioPlayers.forEach { $0.stop() }
    }
    
    func checkPermission() -> Bool {
        if AXIsProcessTrusted() == false {
            let options: NSDictionary = [kAXTrustedCheckOptionPrompt.takeUnretainedValue() as String: true]
            let accessEnabled = AXIsProcessTrustedWithOptions(options)
            return accessEnabled
        }
        return true
    }
    
    func startMonitor() -> Any? {
        guard let url = soundFileURL else {
            print("Sound file not found.")
            return nil
        }
        
        // Listen for keypresses globally
        return NSEvent.addGlobalMonitorForEvents(matching: .keyDown) { _ in
            print("key pressed, coloring: \(self.coloring), pitch: \(self.pitch)")
            
            do {
                let player = try AVAudioPlayer(contentsOf: url)
                player.play()
                self.audioPlayers.append(player)
            } catch {
                print("Error playing sound: \(error.localizedDescription)")
            }
        }
    }
    
    func stopMonitor(event: Any) -> Void {
        NSEvent.removeMonitor(event)
    }
}
