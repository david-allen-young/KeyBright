import SwiftUI

@main
struct KeyBrightApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
                .frame(width: 400, height: 400)
        }
        
        Settings {
            SettingsView()
        }
    }
}
