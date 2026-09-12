#include <pen_stabilizer/stabilizer.hpp>
#include <iostream>

int main() {
    pen_stabilizer::Stabilizer ink;
    ink.reset({true, 12, .120, 4});
    for (unsigned i=0; i<120; ++i) {
        // Synthetic integration example, NOT a physical-pen test.
        if (!ink.append({{double(i), double(i)}, i/120.0, .5f})) return 1;
        // Rebuild your temporary geometry starting at ink.changedBegin().
        // Only indices less than ink.committedPrefix() are immutable.
        // Width/pressure mapping and the renderer belong to your application.
    }
    ink.finish(); // No extra point and no pen-up beautification pass.
    std::cout << pen_stabilizer::package_version << ": "
              << ink.positions().size() << " points\n";
}
