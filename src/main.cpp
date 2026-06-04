#include "raycaster.hpp"

int main(){
    auto raycaster = std::make_unique<Raycaster>();
    raycaster->run();
    return 0;
}