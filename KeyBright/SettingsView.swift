import SwiftUI

enum Colorings: String, CaseIterable {
    case darkest = "darkest"
    case dark = "dark"
    case neutral = "neutral"
    case bright = "bright"
    case brightest  = "brightest"
}

enum Pitches: String, CaseIterable {
    case lowest = "lowest"
    case lower = "lower"
    case neutral = "neutral"
    case higher = "higher"
    case highest  = "highest"
}

struct SettingsView: View {
    var body: some View {
        TabView {
            Tab("General", systemImage: "gear") {
                GeneralSettingsView()
            }
        }
        .scenePadding()
        .frame(width: 600)
    }
}

struct GeneralSettingsView: View {
    @AppStorage("soundFileURL") private var soundFileURL: URL?
    @AppStorage("coloring") private var coloring: String = Colorings.neutral.rawValue
    @AppStorage("pitch") private var pitch: String = Pitches.neutral.rawValue
    
    var body: some View {
        VStack(alignment: .leading) {
            Form {
                Section {
                    VStack(alignment: .leading) {
                        Text("Source Sound")
                            .font(.headline)
                        GroupBox {
                            VStack(spacing: 0) {
                                HStack {
                                    Text("File")
                                    Spacer()
                                    Text(soundFileURL?.lastPathComponent ?? "(not selected)")
                                        .help(soundFileURL?.path ?? "")
                                        .foregroundStyle(soundFileURL != nil ? .primary : .secondary)
                                    Button("Select File...") {
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
                                .padding(5)
                            }
                        }
                    }
                }
                .padding(.bottom, 20)
                
                Section {
                    VStack(alignment: .leading) {
                        Text("Processing")
                            .font(.headline)
                        GroupBox {
                            HStack {
                                Text("Coloring")
                                Spacer()
                                    .frame(maxWidth: .infinity)
                                Picker("Coloring", selection: $coloring) {
                                    ForEach(Colorings.allCases, id: \.self) { coloring in
                                        Text(coloring.rawValue).tag(coloring.rawValue)
                                    }
                                }
                                .pickerStyle(.segmented)
                                .labelsHidden()                            }
                            .padding(5)
                            Divider()
                            HStack {
                                Text("Pitch")
                                Spacer()
                                    .frame(maxWidth: .infinity)
                                Picker("Pitch", selection: $pitch) {
                                    ForEach(Pitches.allCases, id: \.self) { pitch in
                                        Text(pitch.rawValue).tag(pitch.rawValue)
                                    }
                                }
                                .pickerStyle(.segmented)
                                .labelsHidden()
                            }
                            .padding(5)
                        }
                    }
                }
                .padding(.bottom, 0)
            }
        }
        .padding(10)
    }
}
