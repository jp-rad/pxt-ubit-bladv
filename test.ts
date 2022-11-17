// tests go here; this will not be compiled when this package is used as an extension.
input.onButtonPressed(Button.A, function () {
    a = custom.bar(a)
})
input.onButtonPressed(Button.B, function () {
    a = 0
})
let a = 0
a = 0
basic.forever(function () {
    basic.showNumber(a)
    basic.pause(100)
})
