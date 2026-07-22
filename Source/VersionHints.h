/*
███████╗     ██╗███████╗    █████╗ ██╗   ██╗██████╗ ██╗ ██████╗
██╔════╝     ██║██╔════╝   ██╔══██╗██║   ██║██╔══██╗██║██╔═══██╗
███████╗     ██║█████╗     ███████║██║   ██║██║  ██║██║██║   ██║
╚════██║██   ██║██╔══╝     ██╔══██║██║   ██║██║  ██║██║██║   ██║
███████║╚█████╔╝██║███████╗██║  ██║╚██████╔╝██████╔╝██║╚██████╔╝
╚══════╝ ╚════╝ ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═════╝ ╚═╝ ╚═════╝
*/

#pragma once
#include <JuceHeader.h>

namespace sjf::version_hints
{
    // The master map of known parameter IDs to their Version Hints
    // You can delete everything inside this map to regenerate the entire list
    inline static std::unordered_map<std::string, int> hints = {
        // Automatically populated at runtime during Debug builds
    };


    /**
     * @brief Retrieves the version hint for a parameter ID.
     * In Debug mode, if the parameter ID is not found, it updates this source file
     * on disk so the new parameter and its incremented hint are baked into the next build.
     */
    inline int getVersionHint(const juce::String& paramID)
    {
        const std::string idStr = paramID.toStdString();

        // 1. If it already exists in the map, return the saved hint
        if (auto it = hints.find(idStr); it != hints.end())
            return it->second;

        #if JUCE_DEBUG

        static const auto newVersionHint = [&](){
            int maxVersion = 0;
            for (const auto& [id, ver] : hints)
                maxVersion = std::max(maxVersion, ver);

            return maxVersion + 1;
        }();

        hints[idStr] = newVersionHint;

        const juce::File thisHeaderFile (__FILE__);

        if (thisHeaderFile.existsAsFile())
        {
            juce::String content = thisHeaderFile.loadFileAsString();

            // Locate the "hints = {" map block using regex/string searching
            const int mapStart = content.indexOf("hints = {");
            if (mapStart != -1)
            {
                const int insertPos = content.indexOf(mapStart, "\n") + 1;

                const juce::String entryToInsert = "        {\"" + paramID + "\", " + juce::String(newVersionHint) + "},\n";

                if (!content.contains("\"" + paramID + "\""))
                {
                    content = content.substring(0, insertPos) + entryToInsert + content.substring(insertPos);
                    ignoreUnused(thisHeaderFile.replaceWithText(content));

                    DBG("[VersionHints] Registered new parameter '" << paramID
                        << "' with Hint " << newVersionHint << " in " << thisHeaderFile.getFileName());
                }
            }
        }
        return newVersionHint;

        #endif

        return 0;
    }
}
