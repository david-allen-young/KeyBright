import SwiftUI
import AVFoundation
import Cocoa
import Observation
import Accessibility

@Observable
class KeypressMonitor {
    var isEnabled = true
    private var eventMonitor: Any?
    private var localEventMonitor: Any?

    // Use multiple players to avoid delay when keypresses happen in rapid succession
    private var audioPlayers: [AVAudioPlayer] = []
    private var currentPlayerIndex = 0
    private let playerCount = 3  // Number of players in rotation

    init() {
        setUpAudioPlayers()
    }

    func setUpAudioPlayers() {
        let soundPath = NSHomeDirectory() + "/sound.mp3"
        let soundURL = URL(fileURLWithPath: soundPath)

        // Create multiple audio players for the same sound
        for _ in 0..<playerCount {
            do {
                let player = try AVAudioPlayer(contentsOf: soundURL)
                player.prepareToPlay()
                player.volume = 1.0

                // Minimize latency
                player.numberOfLoops = 0
                player.rate = 1.0

                audioPlayers.append(player)
            } catch {
                print("Error loading sound file: \(error)")
            }
        }
    }

    func startMonitoring() {
        if eventMonitor != nil {
            // Already monitoring
            return
        }

        // Monitor any key down events globally (outside our app)
        eventMonitor = NSEvent.addGlobalMonitorForEvents(matching: .keyDown) { [weak self] _ in
            guard let self = self, self.isEnabled else { return }
            self.playSound()
        }

        // Also monitor key events in our app
        localEventMonitor = NSEvent.addLocalMonitorForEvents(matching: .keyDown) { [weak self] event in
            guard let self = self, self.isEnabled else { return event }
            self.playSound()
            return event
        }
    }

    func stopMonitoring() {
        if let eventMonitor = eventMonitor {
            NSEvent.removeMonitor(eventMonitor)
            self.eventMonitor = nil
        }

        if let localEventMonitor = localEventMonitor {
            NSEvent.removeMonitor(localEventMonitor)
            self.localEventMonitor = nil
        }
    }

    func playSound() {
        // Use the next player in rotation to avoid delay
        let player = audioPlayers[currentPlayerIndex]

        // If the current player is still playing, stop it first for quick response
        if player.isPlaying {
            player.stop()
        }

        player.currentTime = 0
        player.play()

        // Move to the next player for the next keypress
        currentPlayerIndex = (currentPlayerIndex + 1) % playerCount
    }

    deinit {
        stopMonitoring()
    }
}

@main
struct KeyBrightApp: App {
    @State private var keypressMonitor = KeypressMonitor()
    @State private var permissionGranted = false
    @Environment(\.openWindow) private var openWindow

    var body: some Scene {
        WindowGroup {
            ContentView(keypressMonitor: keypressMonitor, permissionGranted: $permissionGranted)
                .onAppear {
                    checkAccessibilityPermissions()
                }
        }
        .windowResizability(.contentSize)
        .commands {
            CommandGroup(replacing: .appInfo) {
                Button("About KeyBright") {
                    openWindow(id: "about")
                }
            }
        }

        Window("About", id: "about") {
            AboutView()
        }
        .defaultSize(width: 300, height: 200)
    }

    func checkAccessibilityPermissions() {
        // Use the modern API for checking permissions
        let options: NSDictionary = [kAXTrustedCheckOptionPrompt.takeUnretainedValue() as String: true]
        permissionGranted = AXIsProcessTrustedWithOptions(options)

        if permissionGranted {
            keypressMonitor.startMonitoring()
        }
    }
}

struct ContentView: View {
    var keypressMonitor: KeypressMonitor
    @Binding var permissionGranted: Bool
    @State private var soundFileExists = false
    @State private var showFilePickerSheet = false
    @State private var selectedAudioURL: URL?

    var body: some View {
        VStack(spacing: 20) {
            Text("KeyBright")
                .font(.title)
                .fontWeight(.bold)

            if permissionGranted {
                statusSection
            } else {
                permissionSection
            }

            configSection

            Text("Playing: ~/sound.mp3")
                .foregroundStyle(soundFileExists ? .primary : .secondary)
                .padding(.top, 5)
                .onAppear {
                    checkSoundFileExists()
                }

            Button("Choose Different Sound File") {
                showFilePickerSheet = true
            }
            .buttonStyle(.bordered)
            .fileImporter(
                isPresented: $showFilePickerSheet,
                allowedContentTypes: [.audio],
                allowsMultipleSelection: false
            ) { result in
                switch result {
                case .success(let urls):
                    if let url = urls.first {
                        // Copy the selected file to ~/sound.mp3
                        let destination = URL(fileURLWithPath: NSHomeDirectory() + "/sound.mp3")
                        try? FileManager.default.removeItem(at: destination)
                        try? FileManager.default.copyItem(at: url, to: destination)

                        // Reinitialize audio players
                        keypressMonitor.setUpAudioPlayers()
                        checkSoundFileExists()
                    }
                case .failure(let error):
                    print("File selection failed: \(error)")
                }
            }
        }
        .padding(20)
        .frame(width: 400, height: 400)
    }

    var statusSection: some View {
        VStack {
            Image(systemName: keypressMonitor.isEnabled ? "speaker.wave.3.fill" : "speaker.slash.fill")
                .font(.system(size: 40))
                .foregroundStyle(keypressMonitor.isEnabled ? .green : .secondary)
                .padding()

            Toggle("Enable Sound on Keypress", isOn: Bindable(keypressMonitor).isEnabled)
                .toggleStyle(.switch)
                .padding(.horizontal)
                .onChange(of: keypressMonitor.isEnabled) { _, newValue in
                    if newValue {
                        keypressMonitor.startMonitoring()
                    } else {
                        keypressMonitor.stopMonitoring()
                    }
                }
        }
    }

    var permissionSection: some View {
        VStack {
            Image(systemName: "exclamationmark.shield")
                .font(.system(size: 40))
                .foregroundStyle(.orange)
                .padding()

            Text("Accessibility Permission Required")
                .font(.headline)

            Text("This app needs permission to monitor keypresses")
                .font(.caption)
                .multilineTextAlignment(.center)
                .padding(.bottom)

            Button("Open Security & Privacy Settings") {
                NSWorkspace.shared.open(URL(string: "x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility")!)
            }
            .buttonStyle(.borderedProminent)
        }
    }

    var configSection: some View {
        VStack {
            Divider()
                .padding(.vertical, 10)
        }
    }

    func checkSoundFileExists() {
        let soundPath = NSHomeDirectory() + "/sound.mp3"
        soundFileExists = FileManager.default.fileExists(atPath: soundPath)
    }
}

struct AboutView: View {
    var body: some View {
        VStack(spacing: 15) {
            Text("KeyBright")
                .font(.headline)

            Text("Version 2.0")
                .font(.subheadline)

            Text("© 2025")
                .font(.caption)

            Text("For macOS Sequoia and later")
                .font(.caption2)
                .foregroundStyle(.secondary)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}
