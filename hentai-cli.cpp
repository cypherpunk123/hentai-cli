#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <unistd.h>
using namespace std;

// COLORS
const string c_red="\033[1;31m";
const string c_green="\033[1;32m";
const string c_yellow="\033[1;33m";
const string c_blue="\033[1;34m";
const string c_magenta="\033[1;35m";
const string c_cyan="\033[1;36m";
const string c_reset="\033[0m";

// PROVIDERS
const int MIN_PROVIDER = 1;
const int MAX_PROVIDER = 4;

// FUNCTIONS
void print_help_text() {
    cout << "Usage: hentai-cli [options]" << endl;
    cout << "\thentai-cli [-d | -r <download_dir>] [<query>]" << endl;
    cout << "\thentai-cli [-v] [<query>]" << endl;
    cout << "\thentai-cli -h" << endl;
    cout << "Options:" << endl;
    cout << "\t-h show this help text" << endl;
    cout << "\t-d download episode" << endl;
    cout << "\t-r download episode to directory" << endl;
    cout << "\t-v use VLC as the media player" << endl;
    cout << "\t-p select provider [1: hentaimama, 2: hentaihaven, 3: hanime.tv, 4: hentaigasm]" << endl;
}

void print_error(string error) {
    cerr << c_red << error << c_reset << endl;
}

string get_provider(int provider = 0) {
  if (provider == 0) {
    cout << "Select a provider [1: hentaimama, 2: hentaihaven, 3: hanime.tv, 4: hentaigasm]\n> ";
    string input;
    getline(cin, input);

    while (input.empty() || !isdigit(input[0]) ||
           stoi(input) < MIN_PROVIDER || stoi(input) > MAX_PROVIDER) {
      print_error("Invalid provider");
      cout << "Select a provider [1: hentaimama, 2: hentaihaven, 3: hanime.tv, 4: hentaigasm]\n> ";
      getline(cin, input);
    }
    return input;
  } else {
    return to_string(provider);
  }
}

string get_query(string base_url, string query = "") {
  if (query.empty()) {
    cout << "Searching " << base_url << ": \n> ";
    getline(cin, query);
    while (query.empty()) {
      cout << "\x1B[2J\x1B[H";
      print_error("No query provided");
      cout << "Searching " << base_url << ": \n> ";
      getline(cin, query);
    }
  }
  replace(query.begin(), query.end(), ' ', '+');
  return query;
}

int get_episode_number(int max_episodes) {
  cout << "To specify a range, use: start_number end_number (Episodes: 1-"
            << max_episodes << ")\n> ";
  string input;
  getline(cin, input);

  while (input.empty() || !isdigit(input[0]) ||
         stoi(input) < 1 || stoi(input) > max_episodes) {
    print_error("Invalid episode number");
    cout << "To specify a range, use: start_number end_number (Episodes: 1-"
              << max_episodes << ")\n> ";
    getline(cin, input);
  }
  int ep_no = stoi(input);
  return ep_no;
}

void stream(const string& url, const string& title,
            const string& player_fn) {
  if (player_fn == "vlc") {
    if (system("uname -a | grep -qE \"[Aa]ndroid\"") == 0) {
      string cmd = "am start --user 0 -a android.intent.action.VIEW -d \"" +
                        url +
                        "\" -n org.videolan.vlc/org.videolan.vlc.gui.video.VideoPlayerActivity -e \"title\" \"" +
                        title + "\" > /dev/null 2>&1";
      system(cmd.c_str());
    } else {
      string cmd = "setsid -f \"" + player_fn +
                        "\" --http-referrer=\"base_url\" \"" + url +
                        "\" > /dev/null 2>&1";
      system(cmd.c_str());
    }
  } else if (player_fn == "mpv") {
    if (system("uname -a | grep -qE \"[Aa]ndroid\"") == 0) {
      string cmd = "am start --user 0 -a android.intent.action.VIEW -d \"" +
                        url +
                        "\" -n is.xyz.mpv/.MPVActivity > /dev/null 2>&1";
      system(cmd.c_str());
    } else {
      string cmd = "setsid -f \"" + player_fn + "\" \"" + url +
                        "\" --referrer=\"base_url\" --force-media-title=\"" +
                        title + "\" > /dev/null 2>&1";
      system(cmd.c_str());
    }
  }
}

void download(const string& url, const string& filename) {
  cout << "Downloading " << filename << endl;
  if (url.find(".m3u8") != string::npos) {
    string cmd = "ffmpeg -loglevel print_error -stats -referer \"base_url\" -i \"" +
                      url + "\" -c copy \"" + "download_dir/" + filename +
                      ".mp4\"";
    system(cmd.c_str());
  } else {
    string cmd = "aria2c --summary-interval=0 -x 16 -s 16 --referer=\"base_url\" \"" +
                      url + "\" --dir=download_dir -o \"" + filename +
                      ".mp4\" --download-result=hide";
    system(cmd.c_str());
  }
}

// DRIVER FUNCTIONS

// provider driver function
// int main() {
//   std::string provider = get_provider();
//   std::cout << "Selected provider: " << provider << std::endl;
//   return 0;
// }

// get query driver function
// int main() {
//   std::string query = get_query();
//   std::cout << "Search query: " << query << std::endl;
//   return 0;
// }

// download driver function
// int main() {
//   string url = "http://example.com";
//   string filename = "example";
//   download(url, filename);
//   return 0;
// }

// stream driver function
// int main() {
//   string url = "http://example.com";
//   string title = "Example Title";
//   string player_fn = "vlc";
//   stream(url, title, player_fn);
//   return 0;
// }

// main driver function
int main(){
  string provider = get_provider();
  string base_url = "";
  if (provider == "1") {
    base_url = "https://hentaimama.io";
  } else if (provider == "2") {
    base_url = "https://hentaihaven.xxx";
  } else if (provider == "3") {
    base_url = "https://hanime.tv";
  } else if (provider == "4") {
    base_url = "https://hentaigasm.com";
  }
  string query = get_query(base_url);

  int max_episodes = 10;
  get_episode_number(max_episodes);
  return 0;
}
