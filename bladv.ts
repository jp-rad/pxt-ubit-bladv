
/*
MIT License

Copyright (c) 2022-2023 jp-rad

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

enum TakoSymbols {
    //% block="circle"
    //% jres=icons.chessboard
    Circle = 0,
    //% block="square"
    //% jres=icons.square
    Square,
    //% block="star"
    //% jres=icons.stickfigure
    Star,
    //% block="heart"
    //% jres=icons.heart
    Heart,
    //% block="triangle"
    //% jres=icons.triangle
    Triangle,
}

/**
 * BLADV - Accumulate the Complete list of 16-bit Service UUIDs as an AD structure in the advertising payload for micro:bit.
 */
//% block="BLADV"
//% weight=100 color=#4b0082 icon="\uf09e"
//% groups="['Tako']"
namespace bladv {

    /**
     * Set Complete list of 16-bit Service ID.
     * @param serviceUUID 16-bit Service ID
     */
    //% blockId=bladv_accumulate_complete_list_16bit_service_id
    //% block="BLADV Complete list of 16-bit Service ID: %serviceUUID"
    //% serviceUUID.min=1 serviceUUID.max=65535 serviceUUID.defl=6144
    //% shim=bladv::accumulateCompleteList16BitServiceID
    //% weight=120
    export function accumulateCompleteList16BitServiceID(serviceUUID: number): void
    {
        return;
    }
    
    // for TAKO
    let _counter = -1;
    loops.everyInterval(500, function () {
        if (0 > _counter) {
            // nop
        } else if (0 == _counter) {
            _stopTako();
            _counter = -1;
        } else {
            if (5 == _counter) {
                _zeroTako();
            }
            _counter -= 1;
        }
    })

    /**
     * Send symbol to TAKO.
     * @param symbol 0:circle, 1:square, 2:star, 3:heart, 4:triangle
     */
    //% blockId=bladv_send_to_tako
    //% block="TAKO Send %i"
    //% mark.fieldEditor="imagedropdown"
    //% mark.fieldOptions.columns="1"
    //% mark.fieldOptions.width="76"
    //% mark.fieldOptions.maxRows="5"
    //% weight=100
    //% group="Tako"
    export function sendToTako(symbol: TakoSymbols): void
    {
        _counter = 10;
        _sendToTako(symbol);
    }

    /**
     * Reset TAKO.
     */
    //% blockId=bladv_reset_tako
    //% block="TAKO Reset"
    //% weight=90
    //% group="Tako"
    export function resetTako(): void
    {
        _counter = 5;
    }
        
    /**
     * (shim) Send symbol to TAKO.
     * @param symbol 0:circle, 1:square, 2:star, 3:heart, 4:triangle
     */
    //% block
    //% blockHidden=true
    //% weight=82
    //% group="Tako"
    //% shim=bladv::_sendToTako
    export function _sendToTako(symbol: TakoSymbols): void
    {
        // shim
    }
    
    /**
     * (shim) Zero TAKO.
     */
    //% block
    //% blockHidden=true
    //% weight=81
    //% group="Tako"
    //% shim=bladv::_zeroTako
    export function _zeroTako(): void
    {
        // shim
    }
    
    /**
     * (shim) Stop TAKO.
     */
    //% block
    //% blockHidden=true
    //% weight=80
    //% group="Tako"
    //% shim=bladv::_stopTako
    export function _stopTako(): void
    {
        // shim
    }

}
