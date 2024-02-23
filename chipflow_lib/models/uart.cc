/* SPDX-License-Identifier: BSD-2-Clause */
#include "build/sim/sim_soc.h"
#include "log.h"
#include <cxxrtl/cxxrtl.h>
#include <fstream>
#include <stdexcept>

namespace cxxrtl_design {

struct uart_model : public bb_p_uart__model {
    struct {
        bool tx_last;
        int counter = 0;
        uint8_t sr = 0;
    } s;

    int baud_div = 0;
    uart_model() {
        // TODO: don't hardcode
        baud_div = (25000000)/115200;
    }

    bool eval(performer *performer) override {
        if (posedge_p_clk()) {
            if (s.counter == 0) {
                if (s.tx_last && !p_tx__o) { // start bit
                    s.counter = 1;
                }
            } else {
                ++s.counter;
                if (s.counter > (baud_div / 2) && ((s.counter - (baud_div / 2)) % baud_div) == 0) {
                    int bit = ((s.counter - (baud_div / 2)) / baud_div);
                    if (bit >= 1 && bit <= 8) {
                        // update shift register
                        s.sr = (p_tx__o ? 0x80U : 0x00U) | (s.sr >> 1U);
                    }
                    if (bit == 8) {
                        // print to console
                        log("%c", char(s.sr));
                    }
                    if (bit == 9) {
                        // end
                        s.counter = 0;
                    }
                }
            }
            s.tx_last = bool(p_tx__o);
        }
        return /*converged=*/true;
    }

    ~uart_model() {}
};

std::unique_ptr<bb_p_uart__model> bb_p_uart__model::create(std::string name, metadata_map parameters, metadata_map attributes) {
    return std::make_unique<uart_model>();
}

}
