#include <UnionType>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <variant>
#include <functional>


using namespace std;
using namespace Union;
int main() {
    auto gain = Union::CalculatedGain(1.0, 2.0);
    cout << "Gain: " << gain << endl;
    cout << "output: " << CalculateGainOutput(1.0, 6) << endl;
    cout << "intput: " << CalculateGainInput(2.0, 6) << endl;
    cout << "output: " << KeepDecimals(CalculateGainOutput(1.0, 6)) << endl;
    cout << "intput: " << KeepDecimals(CalculateGainInput(2.0, 6)) << endl;
    cout << "map:" << Union::ValueMap(-100.0, {0.0,100.0}, {0.0,200.0});
    return 0;
}
