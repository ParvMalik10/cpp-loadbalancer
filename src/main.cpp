#include "load_balancer.cpp"

int main() {
    LoadBalancer lb(8080);
    lb.start();
    return 0;
}