import SwiftUI
import AVFoundation

struct ContentView: View {
    @Environment(\.openSettings) private var openSettings
    
    @AppStorage("soundFileURL") private var soundFileURL: URL?
    @AppStorage("coloring") private var coloring: String = Colorings.neutral.rawValue
    @AppStorage("pitch") private var pitch: String = Pitches.neutral.rawValue
    
    @State private var audioPlayers: [AVAudioPlayer] = []
    @State private var permissionEnabled: Bool = false
    @State private var monitoring: Bool = false
    
    var body: some View {
        VStack {
            if monitoring {
                HStack {
                    Image(systemName: "keyboard")
                        .imageScale(.large)
                        .foregroundStyle(.green)
                    Text("On")
                        .foregroundStyle(.green)
                }
            } else {
                HStack {
                    Image(systemName: "exclamationmark.triangle")
                        .imageScale(.large)
                        .foregroundStyle(permissionEnabled ? .secondary : Color.red)
                    Text("System Permission Required")
                        .foregroundStyle(.red)
                }
                Button("Check Permission") {
                    self.permissionEnabled = checkPermission()
                    if permissionEnabled {
                        startMonitor()
                        self.monitoring = true
                    }
                }
                .padding(.top, 10)
                Text("You may need to remove the app from the list then re-add it.")
                    .font(.caption)
                    .foregroundStyle(.secondary)
            }
        }
        .padding()
    
        VStack {
            HStack {
                Text("Coloring: ")
                Text(coloring)
            }
            HStack {
                Text("Pitch: ")
                Text(pitch)
            }
            HStack {
                Text("Sound File: ")
                Text(soundFileURL?.lastPathComponent ?? "Not Set")
            }
            
            Button("Open Settings", systemImage: "gear") {
                openSettings()
            }
        }
        .padding()
    }
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        self.permissionEnabled = checkPermission()
        if permissionEnabled {
            startMonitor()
            self.monitoring = true
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
    
    func startMonitor() {
        guard let url = soundFileURL else {
            print("Sound file not found.")
            return
        }
        
        // Listen for keypresses globally
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.1) {
            NSEvent.addGlobalMonitorForEvents(matching: .keyDown) { _ in
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
    }
}
