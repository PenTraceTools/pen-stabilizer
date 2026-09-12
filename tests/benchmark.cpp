#include <pen_stabilizer/stabilizer.hpp>
#include <chrono>
#include <iostream>
#include <vector>

int main() {
    using namespace pen_stabilizer;
    using Clock = std::chrono::steady_clock;
    for (int hz : {120,672}) {
        std::vector<Sample> points;
        for (int i=0;i<hz*60;++i) {
            const double t=double(i)/hz;
            points.push_back({{20*t,20*t+std::sin(t*30)},t,.5f});
        }
        const auto start=Clock::now();
        Stabilizer stream;
        for (const auto& point:points) stream.append(point);
        const auto middle=Clock::now();
        const auto batch=Stabilizer::filterBatch(points);
        const auto end=Clock::now();
        for (std::size_t i=0;i<batch.size();++i)
            if ((batch[i]-stream.positions()[i]).length()>1e-7) return 1;
        const auto ms=[](auto d){return std::chrono::duration<double,std::milli>(d).count();};
        std::cout<<hz<<" Hz, 60-second synthetic stroke, "<<points.size()
            <<" samples; stream="<<ms(middle-start)<<" ms; batch="<<ms(end-middle)
            <<" ms. Excludes input/UI/mesh rendering.\n";
    }
}
