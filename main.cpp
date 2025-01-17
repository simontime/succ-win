#include <cstdlib>
#include <cstdio>
#include <string>
#include <map>
#include <regex>

#include <Windows.h>

using namespace std::string_literals;

static std::map<std::string,int> gObservedFlags;

static bool isFlag(char const * const arg)
{
	return arg[0] == '-';
}

// [chanting] CONST STAR CONST STAR CONST...
static void handleHelpFlag(const int argc, char const * const * const argv)
{
	for (int i = 1; i < argc; ++i) {
		if (argv[i] == "-help"s || argv[i] == "--help"s || argv[i] == "--help-hidden"s) {
			printf("You will find no help here.\n");
			exit(0);
		}
	}
}

static void handleVersionFlag(const int argc, char const * const * const argv)
{
	for (int i = 1; i < argc; ++i) {
		if (argv[i] == "-v"s || argv[i] == "--version"s) {
			printf("SUCC version 0.0.1\n");
			printf("Target: at least a couple dozen retweets and maybe a 'luna why'\n");
			printf("Thread model: none\n");
			printf("InstalledDir: /dev/circles-of-hell/9\n");
			exit(0);
		}
	}
}

static void checkFilesActuallyExist(const int argc, char const * const * const argv)
{
	bool hasAnyFiles = false;
	bool hasAnyMissingFiles = false;
	for (int i = 1; i < argc; ++i) {
		if (!isFlag(argv[i])) {
			FILE* fh = fopen(argv[i], "rb");
			if (fh) {
				hasAnyFiles = true;
				fclose(fh);
			} else {
				hasAnyMissingFiles = true;
				fprintf(stderr, "succ: error: no such file or directory: '%s'\n", argv[i]);
			}
		}
	}
	if (!hasAnyFiles) {
		fputs("succ: error: no input files\n", stderr);
		exit(1);
	} else if (hasAnyMissingFiles) {
		exit(1);
	}
}

static void handleDuplicateFlag(const std::string& flag)
{
	auto result = gObservedFlags.find(flag);
	if (result != gObservedFlags.end()) {
		switch (result->second) {
			case 1: {
				fprintf(stderr, "warning: Clearly you have very little confidence in your compiler, since you feel the need to specify the same flag twice. [%s]\n", flag.c_str());
				break;
			}
			case 2: {
				fprintf(stderr, "warning: Again?! You really have no trust at all, do you? [%s]\n", flag.c_str());
				break;
			}
			case 3: {
				fprintf(stderr, "warning: Goodness me. I refuse to believe you're this mistrustful. Go check your build scripts, you've surely got a mistake somewhere. [%s]\n", flag.c_str());
				break;
			}
		}
		gObservedFlags[flag]++;
	} else {
		gObservedFlags.emplace(flag, 1);
	}
}

static void handleWarningFlag(const std::string& flag)
{
	if (flag.starts_with("-Wno-") || flag == "-w") {
		fprintf(stderr, "warning: Have confidence in your code. Do not try to disable warnings. [%s]\n", flag.c_str());
		return;
	}

	if (flag == "-Werror" || flag.starts_with("-Werror=")) {
		fprintf(stderr, "succ: error: You asked for an error, so here, have one. [%s]\n", flag.c_str());
		exit(1);
	}
}

static void handleFeatureFlag(const std::string& flag)
{
	// https://twitter.com/___srv_/status/1205722571100041216
	// https://twitter.com/Andrew_Taylor/status/1205764994526265345
	if (std::regex_search(flag, std::regex("^-fpwe+ase$"))) {
		Sleep((flag.length() - 7) * 1000);
		return;
	}

	if (flag == "-fmodules" || flag == "-fmodules-ts") {
		fprintf(stderr, "succ: error: Don't even bother. Modules are broken and you know it. [%s]\n", flag.c_str());
		exit(1);
	}
}

static void handleStdFlag(const std::string& flag)
{
	const std::string version = flag.substr(5);

	if (std::regex_search(version, std::regex("^(c|gnu)\\+\\+(\\d+|2a)$"))) {
		if (version.starts_with("gnu")) {
			fprintf(stderr, "succ: error: SUCC only supports standards-compliant code. We'll have none of those fancy GNU extensions here, thank you. [%s]\n", flag.c_str());
			exit(1);
		} else if (version == "c++98" || version == "c++03") {
			fprintf(stderr, "warning: Bring yourself into the modern day, gramps. [%s]\n", flag.c_str());
			return;
		}
		else if (version == "c++11" || version == "c++14" || version == "c++17" || version == "c++2a") {
			// do nothing
			return;
		}
	}

	fprintf(stderr, "succ: error: What? That's not a real version of the language. [%s]\n", flag.c_str());
	exit(1);
}

static void handleFlag(const std::string& flag)
{
	handleDuplicateFlag(flag);

	if (flag.starts_with("-W") || flag == "-w") {
		handleWarningFlag(flag);
	} else if (flag.starts_with("-f")) {
		handleFeatureFlag(flag);
	} else if (flag.starts_with("-std=")) {
		handleStdFlag(flag);
	}
}

static void handleFile(const std::string& file)
{
	(void)file;
	// TODO: add minimal "parsing" of code to allow for basic shit-talking
}

static void attemptToBeFunny(const int argc, char const * const * const argv)
{
	for (int i = 1; i < argc; ++i) {
		if (isFlag(argv[i])) {
			handleFlag(argv[i]);
		} else {
			handleFile(argv[i]);
		}
	}
}

static void giveUpAndJustSayNo()
{
	fputs("succ: error: no\n", stderr);
}

int main(int argc, char const * const * const argv)
{
	handleHelpFlag(argc, argv);
	handleVersionFlag(argc, argv);
	checkFilesActuallyExist(argc, argv);
	attemptToBeFunny(argc, argv);
	giveUpAndJustSayNo();
	return 1;
}
