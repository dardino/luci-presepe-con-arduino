const EasingFunctions = {
    // no easing, no acceleration
    linear: function (t: number) { return t },
    // accelerating from zero velocity
    easeInQuad: function (t: number) { return t*t },
    // decelerating to zero velocity
    easeOutQuad: function (t: number) { return t*(2-t) },
    // acceleration until halfway, then deceleration
    easeInOutQuad: function (t: number) { return t<.5 ? 2*t*t : -1+(4-2*t)*t },
    // accelerating from zero velocity 
    easeInCubic: function (t: number) { return t*t*t },
    // decelerating to zero velocity 
    easeOutCubic: function (t: number) { return (--t)*t*t+1 },
    // acceleration until halfway, then deceleration 
    easeInOutCubic: function (t: number) { return t<.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1 },
    // accelerating from zero velocity 
    easeInQuart: function (t: number) { return t*t*t*t },
    // decelerating to zero velocity 
    easeOutQuart: function (t: number) { return 1-(--t)*t*t*t },
    // acceleration until halfway, then deceleration
    easeInOutQuart: function (t: number) { return t<.5 ? 8*t*t*t*t : 1-8*(--t)*t*t*t },
    // accelerating from zero velocity
    easeInQuint: function (t: number) { return t*t*t*t*t },
    // decelerating to zero velocity
    easeOutQuint: function (t: number) { return 1+(--t)*t*t*t*t },
    // acceleration until halfway, then deceleration 
    easeInOutQuint: function (t: number) { return t<.5 ? 16*t*t*t*t*t : 1+16*(--t)*t*t*t*t }
}

interface IHSLColor {
    h: number;
    s: number;
    l: number;
}

interface IPoint2D {
    x: number;
    y: number;
}

interface IColoredPoint2D extends IPoint2D {
    fill: string;
}

interface ISun extends IPoint2D {
    strength: number;
}

interface IColorStep {
    at: number,
    use: IHSLColor,
}

const canvas = document.getElementById('canvas') as HTMLCanvasElement;
const ctx = canvas.getContext('2d')!;

const nightColor: IHSLColor = { h: 0, s: 0, l: 0 };
const sunriseColor: IHSLColor = { h: 359, s: 90, l: 43 };
const midDayColor: IHSLColor = { h: 359, s: 0, l: 100 };
const sunsetColor: IHSLColor = { h: 360, s: 66, l: 76 };

const colorSteps = [
    { at: 0, use: sunriseColor },
    { at: 640, use: midDayColor },
    { at: 1000, use: sunsetColor },
];

const minutesInADay = 1840;
const referenceHeight = 150;

const maxDistance = getDistanceBetweenTwoPoints({ x: 0, y: referenceHeight }, { x: minutesInADay, y: referenceHeight / 3 });

window.requestAnimationFrame(draw);

let titty = 0;
let fnTitty = (t: number) => t + 1;

let ledArray = new Array(94).fill(0).map<IColoredPoint2D>((x, i) => ({ x: i * 8 + 550, y: referenceHeight / 3, fill: `hsl(360, 0%, 0%)` }));

function draw() {
    titty = fnTitty(titty);
    const t = titty;
    const sun: ISun = getSunPosition(t);
    sun.strength = getNormalizedSunStrength(sun);
    const sunColor = getSunColor(t, colorSteps);

    ctx.clearRect(0, 0, minutesInADay, referenceHeight);
    ctx.fillStyle = 'darkblue';
    ctx.fillRect(0, 0, minutesInADay, referenceHeight);
    drawSun(sun, sunColor);
    ledArray = computeLedArray(ledArray, sun, sunColor);
    drawLeds(ledArray);

    if (titty > minutesInADay) {
        titty = 0;
    }
    window.requestAnimationFrame(draw);
}

function drawLeds(leds: IColoredPoint2D[]) {
    const h = referenceHeight / 3;

    leds.forEach(led => {
        ctx.beginPath();
        ctx.fillStyle = led.fill;
        ctx.arc(led.x, h, 2, 0, 2 * Math.PI);
        ctx.fill();
        ctx.closePath();
    });
}

function drawSun(sun: ISun, sunColor: IHSLColor) {
    ctx.beginPath();
    ctx.fillStyle = `hsl(${sunColor.h},${sunColor.s}%,${sunColor.l}%)`;
    ctx.arc(sun.x, sun.y, 10, 0, 2*Math.PI);
    ctx.fill();
    ctx.closePath();
}

function getDistanceBetweenTwoPoints(a: IPoint2D, b: IPoint2D) {
    const pit = Math.pow(b.x - a.x, 2) + Math.pow(b.y - a.y, 2);

    return Math.sqrt(pit);
}

function getSunPosition(t: number) {

    const x = t; // <-- to parametrize

    const y = getSunY(normalizeSunX(x));

    return { x, y, strength: 0 };
}

function normalizeSunX(x: number) {
    const min = 0;
    const max = canvas.getBoundingClientRect().width;

    const diff = max - min;
    const translated = x - min;

    return translated / diff;
}

function getSunY(t: number) {
    return EasingFunctions.easeInOutQuad(Math.abs((t - 0.5) * 2)) * referenceHeight;
}

function getNormalizedSunStrength(sun: ISun) {
    const t = 1 - Math.min(referenceHeight * 2 / 3, sun.y) / (referenceHeight * 2 / 3);

    const pow = Math.min(t * 2, 1.5);
    return pow;
}

function computeLedArray(leds: IColoredPoint2D[], sun: ISun, sunColor: IHSLColor) {
    return leds.map(led => {
        return { x: led.x, y: led.y, fill: getHslLedColor(led, sun, sunColor) };
    });
}


function getHslLedColor(led: IColoredPoint2D, sun: ISun, sunColor: IHSLColor): string {
    const dist = Math.abs(getDistanceBetweenTwoPoints(sun, led));
    const normalizedDist = ((dist / maxDistance));

    const l = (1 - normalizedDist) * sun.strength;

    const min = 550 / minutesInADay;
    const max = (550 + 94 * 8) / minutesInADay;

    const s = (normalizedDist) / (max - min);

    if (led.x === 550) {
        console.log(normalizedDist, min, max, s);
    }

    return `hsl(${sunColor.h},${(1 - s) * 100}%,${(l) * 100}%)`;
}

function getSunColor(t: number, steps: IColorStep[]) {
    const step = steps.find((e, i, a) => i < steps.length - 1 ? a[i + 1].at >= t : true);

    return step && step.use || midDayColor;
}

window.onkeydown = (e: KeyboardEvent) => {
    fnTitty = e.keyCode === 37 ? n => n - 1 : n => n + 1;
}
