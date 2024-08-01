#include "PrimeFinder.h"

#include <string>


/*
* Get n value to compute the primes.
*/
int get_n(int argc, char** argv, int* n, int* p, std::string& method) {
    TCLAP::CmdLine cmd("Getting example number", ' ', "1.0");
    TCLAP::ValueArg<int> nArg("n", "number", "n value", true, 1, "int");
    TCLAP::ValueArg<std::string> methodArg("m", "method", "Name of method to be used", true, "seq", "string");
    TCLAP::ValueArg<int> pArg("p", "processors", "Number of processors used", false, 1, "int");
    cmd.add(nArg);
    cmd.add(pArg);
    cmd.add(methodArg);
    cmd.parse(argc, argv);
    *n = nArg.getValue();
    *p = pArg.getValue();
    method = methodArg.getValue();
    return 0;
}

void print_primes(std::vector<int> primes) {
    std::cout << "List of prime numers:\n";

    for (std::vector<int>::iterator it = primes.begin(); it != primes.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << std::endl;
}

std::vector<int> sequential(int n) {
    PrimeFinder pf;
    return pf.getListOfPrimes(n);
}

std::vector<int> multiple_sequential(int n, int p) {
    int m = (n+1) / p;
    int rem = (n+1) % p;
    std::vector<int> primes;
    std::vector<int> n2b_primed;
    
    for (int i = 0; i < p; i++) {
        n2b_primed.push_back(m * (i + 1) - 1);
    }
    if (rem > 0) {
        int new_val = n2b_primed.at(n2b_primed.size() - 1) + rem;
        n2b_primed.pop_back();
        n2b_primed.push_back(new_val);
    }
    int idx = 1, proc_n = 0;
    for (std::vector<int>::iterator it = n2b_primed.begin(); it != n2b_primed.end(); ++it) {
        std::cout << "processing primes from " << idx
            << " to " << *it
            << " by processor " << proc_n << std::endl;
        PrimeFinder2 pf2;
        primes = pf2.getListOfPrimes(idx, *it);
        print_primes(primes);

        idx = *it+1;
        proc_n++;
    }

    return primes;
}

int main(int argc, char* argv[])
{
    int n, p;
    std::string method;

    std::cout << "Computing primes\n";
    get_n(argc, argv, &n, &p, method);
    std::cout << "running " << method << " with n = " << n << " and p = " << p << std::endl;

    if (method == "seq") {
        print_primes(sequential(n));
    }
    else if (method == "mseq") {
        print_primes(multiple_sequential(n, p));
    }
    else {
        std::cout << "Unrecognized method: " << method << std::endl;
        exit(-1);
    }
    exit(0);
}

