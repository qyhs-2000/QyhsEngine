
#include <filesystem>
#include <thread>

#include "include/editor.h"


int main(int argc, char** argv)
{
	std::filesystem::path executable_path(argv[0]);
	std::filesystem::path config_file_path = executable_path.parent_path() / "QyhsEditor.ini";
	
	QYHS::QyhsEditor* editor = new QYHS::QyhsEditor();
	editor->setConfigFile(config_file_path.generic_string());
	editor->run();
	editor->clear();
	return 0;
}
