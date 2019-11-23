const EasingFunctions = {
    // no easing, no acceleration
    linear: function (t) { return t },
    // accelerating from zero velocity
    easeInQuad: function (t) { return t*t },
    // decelerating to zero velocity
    easeOutQuad: function (t) { return t*(2-t) },
    // acceleration until halfway, then deceleration
    easeInOutQuad: function (t) { return t<.5 ? 2*t*t : -1+(4-2*t)*t },
    // accelerating from zero velocity 
    easeInCubic: function (t) { return t*t*t },
    // decelerating to zero velocity 
    easeOutCubic: function (t) { return (--t)*t*t+1 },
    // acceleration until halfway, then deceleration 
    easeInOutCubic: function (t) { return t<.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1 },
    // accelerating from zero velocity 
    easeInQuart: function (t) { return t*t*t*t },
    // decelerating to zero velocity 
    easeOutQuart: function (t) { return 1-(--t)*t*t*t },
    // acceleration until halfway, then deceleration
    easeInOutQuart: function (t) { return t<.5 ? 8*t*t*t*t : 1-8*(--t)*t*t*t },
    // accelerating from zero velocity
    easeInQuint: function (t) { return t*t*t*t*t },
    // decelerating to zero velocity
    easeOutQuint: function (t) { return 1+(--t)*t*t*t*t },
    // acceleration until halfway, then deceleration 
    easeInOutQuint: function (t) { return t<.5 ? 16*t*t*t*t*t : 1+16*(--t)*t*t*t*t }
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

const canvas = document.getElementById('canvas') as HTMLCanvasElement;
const ctx = canvas.getContext('2d')!;

const nightColor: IHSLColor = { h: 0, s: 0, l: 0 };
const sunriseColor: IHSLColor = { h: 359, s: 66, l: 76 };
const midDayColor: IHSLColor = { h: 359, s: 0, l: 100 };
const sunsetColor: IHSLColor = { h: 360, s: 90, l: 43 };

const minutesInADay = 1840;
const referenceHeight = 150;

const maxDistance = getDistanceBetweenTwoPoints({ x: 0, y: referenceHeight }, { x: minutesInADay, y: referenceHeight / 3 });

window.requestAnimationFrame(draw);

let titty = 0;

let ledArray = new Array(94).fill(0).map<IColoredPoint2D>((x, i) => ({ x: i * 8 + 550, y: referenceHeight / 3, fill: `hsl(360, 0%, 0%)` }));

function draw() {
    const t = titty++;
    const sun: ISun = getSunPosition(t);
    sun.strength = getNormalizedSunStrength(sun);

    ctx.clearRect(0, 0, minutesInADay, referenceHeight);
    ctx.fillStyle = 'darkblue';
    ctx.fillRect(0, 0, minutesInADay, referenceHeight);
    drawSun(sun);
    ledArray = computeLedArray(ledArray, sun);
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

function drawSun(sun: ISun) {
    ctx.beginPath();
    ctx.fillStyle = 'red';
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
    console.log(pow);
    return pow;
}

function computeLedArray(leds: IColoredPoint2D[], sun: ISun) {
    return leds.map(led => {
        return { x: led.x, y: led.y, fill: getHslLedColor(led, sun) };
    })
}


function getHslLedColor(led: IColoredPoint2D, sun: ISun): string {
    const dist = Math.abs(getDistanceBetweenTwoPoints(sun, led));
    const invertedDist = ((dist / maxDistance));

    const l = (1 - invertedDist) * sun.strength;


    return `hsl(${360},${100}%,${(l) * 100}%)`;
}

