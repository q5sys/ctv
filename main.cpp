#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <memory>
#include <unordered_set>

namespace fs = std::filesystem;

// Define color pairs
#define COLOR_DIRECTORY 1
#define COLOR_TEXT_FILE 2

// Function to initialize the set of supported file extensions
std::unordered_set<std::string> initSupportedExtensions() {
    std::unordered_set<std::string> extensions;
    
    // Add all supported extensions with dot prefix
    // Common text and config files
    extensions.insert(".txt");
    extensions.insert(".md");
    extensions.insert(".markdown");
    extensions.insert(".conf");
    extensions.insert(".ini");
    extensions.insert(".cfg");
    extensions.insert(".properties");
    
    // Shell scripts
    extensions.insert(".sh");
    extensions.insert(".bash");
    extensions.insert(".ksh");
    extensions.insert(".csh");
    extensions.insert(".zsh");
    
    // Programming languages
    extensions.insert(".c");
    extensions.insert(".h");
    extensions.insert(".cpp");
    extensions.insert(".hpp");
    extensions.insert(".cc");
    extensions.insert(".cxx");
    extensions.insert(".cs");
    extensions.insert(".java");
    extensions.insert(".py");
    extensions.insert(".rb");
    extensions.insert(".js");
    extensions.insert(".ts");
    extensions.insert(".php");
    extensions.insert(".pl");
    extensions.insert(".pm");
    extensions.insert(".go");
    extensions.insert(".rs");
    extensions.insert(".swift");
    extensions.insert(".lua");
    extensions.insert(".r");
    extensions.insert(".scala");
    extensions.insert(".groovy");
    extensions.insert(".kt");
    extensions.insert(".dart");
    
    // Web development
    extensions.insert(".html");
    extensions.insert(".htm");
    extensions.insert(".css");
    extensions.insert(".scss");
    extensions.insert(".sass");
    extensions.insert(".less");
    extensions.insert(".json");
    extensions.insert(".xml");
    extensions.insert(".svg");
    extensions.insert(".jsx");
    extensions.insert(".tsx");
    
    // Data formats
    extensions.insert(".csv");
    extensions.insert(".yaml");
    extensions.insert(".yml");
    extensions.insert(".toml");
    
    // Documentation
    extensions.insert(".rst");
    extensions.insert(".adoc");
    extensions.insert(".tex");
    extensions.insert(".man");
    
    // Build and project files
    extensions.insert(".pro");
    extensions.insert(".cmake");
    extensions.insert(".make");
    extensions.insert(".mk");
    extensions.insert(".mak");
    extensions.insert(".gradle");
    extensions.insert(".pom");
    
    // Other common text-based files
    extensions.insert(".log");
    extensions.insert(".diff");
    extensions.insert(".patch");
    extensions.insert(".sql");
    
    return extensions;
}

// Structure to hold file/directory information
struct FileEntry {
    std::string name;
    bool isDirectory;
    uintmax_t size;
    
    FileEntry(const std::string& n, bool isDir, uintmax_t s = 0)
        : name(n), isDirectory(isDir), size(s) {}
};

// Class to manage the file browser
class FileBrowser {
private:
    std::string currentPath;
    std::vector<FileEntry> entries;
    int selectedIndex = 0;
    int startIndex = 0;
    int maxDisplayEntries = 0;
    WINDOW* window;
    std::unordered_set<std::string> supportedExtensions;
    
public:
    FileBrowser(WINDOW* win) : window(win) {
        currentPath = fs::current_path().string();
        
        // Initialize supported extensions
        supportedExtensions = initSupportedExtensions();
        
        refreshEntries();
    }
    
    void refreshEntries() {
        entries.clear();
        
        // Add parent directory entry
        entries.push_back(FileEntry("..", true));
        
        // Add directories and files
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            if (entry.is_directory()) {
                entries.push_back(FileEntry(entry.path().filename().string(), true));
            } else if (entry.is_regular_file()) {
                // Get file extension and check if it's supported
                std::string ext = entry.path().extension().string();
                if (supportedExtensions.find(ext) != supportedExtensions.end()) {
                    entries.push_back(FileEntry(entry.path().filename().string(), false, entry.file_size()));
                }
            }
        }
        
        // Sort entries: directories first, then files
        std::sort(entries.begin() + 1, entries.end(), 
                 [](const FileEntry& a, const FileEntry& b) {
                     if (a.isDirectory != b.isDirectory) {
                         return a.isDirectory > b.isDirectory;
                     }
                     return a.name < b.name;
                 });
                 
        // Reset selection
        if (selectedIndex >= entries.size()) {
            selectedIndex = entries.size() - 1;
        }
        if (selectedIndex < 0) {
            selectedIndex = 0;
        }
    }
    
    void display() {
        int height, width;
        getmaxyx(window, height, width);
        
        // Calculate max entries that can be displayed
        maxDisplayEntries = height - 3; // Reserve 3 lines for header
        
        // Adjust startIndex if needed
        if (selectedIndex < startIndex) {
            startIndex = selectedIndex;
        } else if (selectedIndex >= startIndex + maxDisplayEntries) {
            startIndex = selectedIndex - maxDisplayEntries + 1;
        }
        
        // Clear window
        werase(window);
        
        // Draw border
        box(window, 0, 0);
        
        // Display current path
        std::string pathDisplay = " " + currentPath + " ";
        if (pathDisplay.length() > width - 4) {
            pathDisplay = " ..." + pathDisplay.substr(pathDisplay.length() - width + 8) + " ";
        }
        mvwprintw(window, 0, 2, "%s", pathDisplay.c_str());
        
        // Display column headers
        mvwprintw(window, 1, 2, "%-*s %10s", width - 15, "Name", "Size");
        mvwhline(window, 2, 1, ACS_HLINE, width - 2);
        
        // Display entries
        int displayCount = std::min(maxDisplayEntries, static_cast<int>(entries.size()));
        for (int i = 0; i < displayCount; i++) {
            int entryIndex = startIndex + i;
            if (entryIndex >= entries.size()) break;
            
            const FileEntry& entry = entries[entryIndex];
            
            // Highlight selected entry
            if (entryIndex == selectedIndex) {
                wattron(window, A_REVERSE);
            }
            
            // Format entry display
            std::string entryDisplay;
            if (entry.isDirectory) {
                // Apply blue color for directories
                wattron(window, COLOR_PAIR(COLOR_DIRECTORY));
                entryDisplay = "[" + entry.name + "]";
                mvwprintw(window, i + 3, 2, "%-*s %10s", width - 15, entryDisplay.c_str(), "<DIR>");
                wattroff(window, COLOR_PAIR(COLOR_DIRECTORY));
            } else {
                // Apply green color for text files
                wattron(window, COLOR_PAIR(COLOR_TEXT_FILE));
                mvwprintw(window, i + 3, 2, "%-*s %10lu", width - 15, entry.name.c_str(), entry.size);
                wattroff(window, COLOR_PAIR(COLOR_TEXT_FILE));
            }
            
            if (entryIndex == selectedIndex) {
                wattroff(window, A_REVERSE);
            }
        }
        
        // Refresh window
        wrefresh(window);
    }
    
    void moveUp() {
        if (selectedIndex > 0) {
            selectedIndex--;
            display();
        }
    }
    
    void moveDown() {
        if (selectedIndex < entries.size() - 1) {
            selectedIndex++;
            display();
        }
    }
    
    bool enter() {
        if (entries.empty()) return false;
        
        const FileEntry& selected = entries[selectedIndex];
        
        if (selected.isDirectory) {
            // Navigate to directory
            if (selected.name == "..") {
                fs::path parent = fs::path(currentPath).parent_path();
                if (!parent.empty()) {
                    currentPath = parent.string();
                }
            } else {
                currentPath = (fs::path(currentPath) / selected.name).string();
            }
            
            selectedIndex = 0;
            startIndex = 0;
            refreshEntries();
            display();
            return false;
        } else {
            // Selected a file
            return true;
        }
    }
    
    std::string getSelectedFilePath() {
        if (selectedIndex >= 0 && selectedIndex < entries.size() && !entries[selectedIndex].isDirectory) {
            return (fs::path(currentPath) / entries[selectedIndex].name).string();
        }
        return "";
    }
};

// Class to display file contents
class FileViewer {
private:
    WINDOW* window;
    std::string filePath;
    std::vector<std::string> lines;
    int startLine = 0;
    int maxDisplayLines = 0;
    
public:
    FileViewer(WINDOW* win) : window(win) {}
    
    bool loadFile(const std::string& path) {
        filePath = path;
        lines.clear();
        startLine = 0;
        
        // Try to open the file
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }
        
        // Read file line by line
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        
        return true;
    }
    
    void display() {
        int height, width;
        getmaxyx(window, height, width);
        
        // Calculate max lines that can be displayed
        maxDisplayLines = height - 3; // Reserve 3 lines for header
        
        // Clear window
        werase(window);
        
        // Draw border
        box(window, 0, 0);
        
        // Display file name
        std::string fileDisplay = " " + fs::path(filePath).filename().string() + " ";
        if (fileDisplay.length() > width - 4) {
            fileDisplay = fileDisplay.substr(0, width - 7) + "... ";
        }
        mvwprintw(window, 0, 2, "%s", fileDisplay.c_str());
        
        // Display line number header
        mvwprintw(window, 1, 2, "Line");
        mvwhline(window, 2, 1, ACS_HLINE, width - 2);
        
        // Display file contents
        if (lines.empty()) {
            mvwprintw(window, 3, 2, "(Empty file)");
        } else {
            int displayCount = std::min(maxDisplayLines, static_cast<int>(lines.size() - startLine));
            for (int i = 0; i < displayCount; i++) {
                int lineIndex = startLine + i;
                if (lineIndex >= lines.size()) break;
                
                // Format line number
                mvwprintw(window, i + 3, 2, "%4d", lineIndex + 1);
                
                // Display line content (truncate if too long)
                std::string lineContent = lines[lineIndex];
                if (lineContent.length() > width - 8) {
                    lineContent = lineContent.substr(0, width - 11) + "...";
                }
                mvwprintw(window, i + 3, 7, "%s", lineContent.c_str());
            }
        }
        
        // Refresh window
        wrefresh(window);
    }
    
    void scrollUp() {
        if (startLine > 0) {
            startLine--;
            display();
        }
    }
    
    void scrollDown() {
        if (startLine < lines.size() - maxDisplayLines) {
            startLine++;
            display();
        }
    }
    
    void pageUp() {
        startLine = std::max(0, startLine - maxDisplayLines);
        display();
    }
    
    void pageDown() {
        startLine = std::min(static_cast<int>(lines.size() - maxDisplayLines), 
                            startLine + maxDisplayLines);
        if (startLine < 0) startLine = 0;
        display();
    }
};

// Main application class
class Application {
private:
    WINDOW* browserWindow;
    WINDOW* viewerWindow;
    FileBrowser* browser;
    FileViewer* viewer;
    bool running = true;
    bool browserFocus = true; // Track which panel has focus
    
public:
    Application() {
        // Initialize ncurses
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0); // Hide cursor
        
        // Initialize colors if terminal supports them
        if (has_colors()) {
            start_color();
            init_pair(COLOR_DIRECTORY, COLOR_BLUE, COLOR_BLACK);
            init_pair(COLOR_TEXT_FILE, COLOR_GREEN, COLOR_BLACK);
        }
        
        // Create windows
        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);
        
        // Reserve the bottom line for help menu
        browserWindow = newwin(maxY - 1, maxX / 2, 0, 0);
        viewerWindow = newwin(maxY - 1, maxX / 2, 0, maxX / 2);
        
        // Create browser and viewer
        browser = new FileBrowser(browserWindow);
        viewer = new FileViewer(viewerWindow);
        
        // Display initial state
        browser->display();
        
        // Highlight the active panel (browser by default)
        wattron(browserWindow, A_BOLD);
        box(browserWindow, 0, 0);
        wattroff(browserWindow, A_BOLD);
        wrefresh(browserWindow);
        
        // Display help at the bottom
        updateHelpMenu();
    }
    
    void updateHelpMenu() {
        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);
        
        // Clear the bottom line
        move(maxY - 1, 0);
        clrtoeol();
        
        // Display help at the bottom
        attron(A_REVERSE);
        mvprintw(maxY - 1, 0, "Tab: Switch panels | Up/Down: Navigate/Scroll | Enter: Open | q: Quit");
        attroff(A_REVERSE);
        
        // Fill the rest of the line with spaces (for consistent highlighting)
        int helpLen = 66; // Length of the help text
        for (int i = helpLen; i < maxX; i++) {
            mvaddch(maxY - 1, i, ' ' | A_REVERSE);
        }
        
        refresh();
    }
    
    ~Application() {
        delete browser;
        delete viewer;
        delwin(browserWindow);
        delwin(viewerWindow);
        endwin();
    }
    
    void run() {
        // Make sure the help menu is always visible
        updateHelpMenu();
        
        // Make sure the browser and viewer are displayed initially
        browser->display();
        
        // Highlight the active panel (browser by default)
        wattron(browserWindow, A_BOLD);
        box(browserWindow, 0, 0);
        wattroff(browserWindow, A_BOLD);
        wrefresh(browserWindow);
        
        // Make sure the viewer window is properly displayed too
        box(viewerWindow, 0, 0);
        wrefresh(viewerWindow);
        
        // Refresh the help menu
        updateHelpMenu();
        
        // Refresh the screen to ensure everything is displayed
        doupdate();
        
        while (running) {
            int ch = getch();
            
            switch (ch) {
                case '\t': // Tab key - switch focus
                    browserFocus = !browserFocus;
                    
                    // Highlight the active panel by redrawing both
                    if (browserFocus) {
                        // Add highlight to browser window title
                        wattron(browserWindow, A_BOLD);
                        box(browserWindow, 0, 0);
                        wattroff(browserWindow, A_BOLD);
                        wrefresh(browserWindow);
                        
                        // Remove highlight from viewer window
                        box(viewerWindow, 0, 0);
                        wrefresh(viewerWindow);
                    } else {
                        // Add highlight to viewer window title
                        wattron(viewerWindow, A_BOLD);
                        box(viewerWindow, 0, 0);
                        wattroff(viewerWindow, A_BOLD);
                        wrefresh(viewerWindow);
                        
                        // Remove highlight from browser window
                        box(browserWindow, 0, 0);
                        wrefresh(browserWindow);
                    }
                    
                    // Ensure help menu stays visible
                    updateHelpMenu();
                    break;
                    
                case KEY_UP:
                    if (browserFocus) {
                        browser->moveUp();
                    } else {
                        viewer->scrollUp();
                    }
                    break;
                    
                case KEY_DOWN:
                    if (browserFocus) {
                        browser->moveDown();
                    } else {
                        viewer->scrollDown();
                    }
                    break;
                    
                case 10: // Enter key
                    if (browserFocus && browser->enter()) {
                        // File selected, load it in viewer
                        std::string filePath = browser->getSelectedFilePath();
                        if (!filePath.empty() && viewer->loadFile(filePath)) {
                            viewer->display();
                        }
                    }
                    break;
                    
                case KEY_PPAGE: // Page Up
                    viewer->pageUp();
                    break;
                    
                case KEY_NPAGE: // Page Down
                    viewer->pageDown();
                    break;
                    
                case 'q':
                case 'Q':
                    running = false;
                    break;
                    
                default:
                    // Ensure help menu stays visible after any action
                    updateHelpMenu();
                    break;
            }
        }
    }
};

int main() {
    try {
        Application app;
        app.run();
        return 0;
    } catch (const std::exception& e) {
        endwin(); // Ensure terminal is restored
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
