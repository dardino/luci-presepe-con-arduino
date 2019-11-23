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

const canvas = document.getElementById('canvas') as HTMLCanvasElement;
const ctx = canvas.getContext('2d');

const nightColor = { h: 0, s: 0, l: 0 };
const sunriseColor = { h: 359, s: 66, l: 76 };
const midDayColor = { h: 359, s: 0, l: 100 };
const sunsetColor = { h: 360, s: 90, l: 43 };

const maxDistance = getDistanceBetweenTwoPoints({ x: 0, y: 150 }, { x: 1640, y: 50});

window.requestAnimationFrame(draw);

let titty = 0;

let ledArray = new Array(94).fill(0).map((x, i) => ({ x: i * 8 + 550, y: 50, fill: `hsl(360, 0%, 0%)` }));

function draw() {
    const t = titty++;
    const sun = getSunPosition(t);
    sun.strength = getNormalizedSunStrength(sun);

    ctx.clearRect(0, 0, 1840, 150);
    ctx.fillStyle = 'darkblue';
    ctx.fillRect(0, 0, 1840, 150);
    drawSun(sun);
    ledArray = computeLedArray(ledArray, sun);
    drawLeds(ledArray);

    if (titty > 1840) {
        titty = 0;
    }
    window.requestAnimationFrame(draw);
}

function drawLeds(leds) {
    const h = 50;

    leds.forEach(led => {
        ctx.beginPath();
        ctx.fillStyle = led.fill;
        ctx.arc(led.x, h, 2, 0, 2 * Math.PI);
        ctx.fill();
        ctx.closePath();
    });
}

function drawSun(sun) {
    ctx.beginPath();
    ctx.fillStyle = 'red';
    ctx.arc(sun.x, sun.y, 10, 0, 2*Math.PI);
    ctx.fill();
    ctx.closePath();
}

function getDistanceBetweenTwoPoints(a, b) {
    const pit = Math.pow(b.x - a.x, 2) + Math.pow(b.y - a.y, 2);

    return Math.sqrt(pit);
}

function getSunPosition(t) {

    const x = t; // <-- to parametrize

    const y = getSunY(normalizeSunX(x));

    return { x, y, strength: 0 };
}

function normalizeSunX(x) {
    const min = 0;
    const max = canvas.getBoundingClientRect().width;

    const diff = max - min;
    const translated = x - min;

    return translated / diff;
}

function getSunY(t) {
    return EasingFunctions.easeInOutQuad(Math.abs((t - 0.5) * 2)) * 150;
}

function getNormalizedSunStrength(sun) {
    const t = 1 - Math.min(100, sun.y) / 100;

    const pow = Math.min(t * 2, 1.5);
    console.log(pow);
    return pow;
}

function computeLedArray(leds, sun) {
    return leds.map(led => {
        return { x: led.x, y: led.y, fill: getHslLedColor(led, sun) };
    })
}


function getHslLedColor(led, sun): string {
    const dist = Math.abs(getDistanceBetweenTwoPoints(sun, led));
    const invertedDist = ((dist / maxDistance));

    const l = (1 - invertedDist) * sun.strength;


    return `hsl(${360},${100}%,${(l) * 100}%)`;
}

