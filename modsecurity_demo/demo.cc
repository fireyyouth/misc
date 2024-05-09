#include <iostream>
#include <assert.h>

#include <modsecurity/modsecurity.h>
#include <modsecurity/rules.h>
#include <modsecurity/transaction.h>

#include "httplib.h"

void detect(modsecurity::ModSecurity *modsec, modsecurity::Rules *rules, const httplib::Request &req, httplib::Response &res) {
    modsecurity::Transaction tx(modsec, rules, nullptr);
    tx.processURI(req.path.data(), req.method.data(), req.version.data());
    for (auto &[k, v]: req.headers) {
        tx.addRequestHeader(k, v);
    }
    tx.processRequestHeaders();

    modsecurity::ModSecurityIntervention it;
    memset(&it, 0, sizeof(it));
    it.status = 200;
    if (tx.intervention(&it)) {
        std::cout << "There is an intervention" << std::endl;
        printf("(status=%d, pause=%d, dis=%d)\n", it.status, it.pause, it.disruptive);
    }

    res.status = it.status;
    if (res.status == 200) {
        res.set_content("Hello World!", "text/plain");
    }

}

int main() {
    modsecurity::ModSecurity modsec;
    modsecurity::Rules rules;
    
    int rule_cnt = rules.loadFromUri("rules.conf");
    std::cerr << rules.m_parserError.str() << '\n';
    rules.dump();
    if (rule_cnt <= 0) {
        std::cerr << "load fail " << rule_cnt << std::endl;
        return -1;
    }


    httplib::Server svr;
    svr.Get("/hi", [&modsec, &rules](const httplib::Request &req, httplib::Response &res) {
            detect(&modsec, &rules, req, res);
    });
    svr.listen("0.0.0.0", 8080);

    
}
