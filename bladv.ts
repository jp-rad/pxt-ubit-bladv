
/*
MIT License

Copyright (c) 2022 jp-rad

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


/**
 * BLADV - Accumulate the Complete list of 16-bit Service UUIDs as an AD structure in the advertising payload for micro:bit.
 */
//% block="BLADV"
//% weight=100 color=#4b0082 icon="\uf09e"
namespace bladv {
    
    /**
     * Set Complete list of 16-bit Service ID.
     * @param serviceUUID 16-bit Service ID
     */
    //% blockId=bladv_accumulate_complete_list_16bit_service_id
    //% block="BLADV Complete list of 16-bit Service ID: %serviceUUID"
    //% serviceUUID.min=1 serviceUUID.max=65535 serviceUUID.defl=6144
    //% shim=bladv::accumulateCompleteList16BitServiceID
    export function accumulateCompleteList16BitServiceID(serviceUUID: number): void
    {
        return;
    }

}
