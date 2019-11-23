const EasingFunctions = {
  // no easing, no acceleration
  linear: (t: number) => t,
  // accelerating from zero velocity
  easeInQuad: (t: number) => t * t,
  // decelerating to zero velocity
  easeOutQuad: (t: number) => t * (2 - t),
  // acceleration until halfway, then deceleration
  easeInOutQuad: (t: number) => (t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t),
  // accelerating from zero velocity
  easeInCubic: (t: number) => t * t * t,
  // decelerating to zero velocity
  easeOutCubic: (t: number) => --t * t * t + 1,
  // acceleration until halfway, then deceleration
  easeInOutCubic: (t: number) =>
    t < 0.5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1,
  // accelerating from zero velocity
  easeInQuart: (t: number) => t * t * t * t,
  // decelerating to zero velocity
  easeOutQuart: (t: number) => 1 - --t * t * t * t,
  // acceleration until halfway, then deceleration
  easeInOutQuart: (t: number) =>
    t < 0.5 ? 8 * t * t * t * t : 1 - 8 * --t * t * t * t,
  // accelerating from zero velocity
  easeInQuint: (t: number) => t * t * t * t * t,
  // decelerating to zero velocity
  easeOutQuint: (t: number) => 1 + --t * t * t * t * t,
  // acceleration until halfway, then deceleration
  easeInOutQuint: (t: number) =>
    t < 0.5 ? 16 * t * t * t * t * t : 1 + 16 * --t * t * t * t * t
};

interface IHSLColor {
  h: number;
  s: number;
  l: number;
}

interface IPoint2D {
  x: number;
  y: number;
}

interface Hsl {
  h: number;
  s: number;
  l: number;
}

interface IColoredPoint2D extends IPoint2D {
  hsl: Hsl;
}

interface ISun extends IPoint2D {
  strength: number;
}

function go(ctx: CanvasRenderingContext2D) {
  const nightColor: IHSLColor = { h: 0, s: 0, l: 0 };
  const sunriseColor: IHSLColor = { h: 359, s: 66, l: 76 };
  const midDayColor: IHSLColor = { h: 359, s: 0, l: 100 };
  const sunsetColor: IHSLColor = { h: 360, s: 90, l: 43 };

  const minutesInADay = 1840;
  const referenceHeight = 150;

  const maxDistance = getDistanceBetweenTwoPoints(
    { x: 0, y: referenceHeight },
    { x: minutesInADay, y: referenceHeight / 3 }
  );

  window.requestAnimationFrame(draw);

  let titty = 0;

  let ledArray = new Array(94).fill(0).map<IColoredPoint2D>((x, i) => ({
    x: i * 8 + 550,
    y: referenceHeight / 3,
    hsl: { h: 360, s: 0, l: 0 }
  }));

  function draw() {
    const t = titty++;
    const sun: ISun = getSunPosition(t);
    sun.strength = getNormalizedSunStrength(sun);

    ctx.clearRect(0, 0, minutesInADay, referenceHeight);
    ctx.fillStyle = "darkblue";
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
      ctx.fillStyle = toHslString(led.hsl);
      ctx.fillRect(led.x - 1, 150, 2, - led.hsl.l * 150);
      ctx.arc(led.x, h, 2, 0, 2 * Math.PI);
      ctx.fill();
      ctx.closePath();
    });
  }

  function drawSun(sun: ISun) {
    ctx.beginPath();
    ctx.fillStyle = "red";
    ctx.arc(sun.x, sun.y, 10, 0, 2 * Math.PI);
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
    const v = Math.abs((t - 0.5) * 2);
    return EasingFunctions.easeInOutQuad(v) * referenceHeight;
  }

  function getNormalizedSunStrength(sun: ISun) {
    const t =
      1 -
      Math.min((referenceHeight * 2) / 3, sun.y) / ((referenceHeight * 2) / 3);

    const pow = Math.min(t * 2, 1.5);
    console.log(pow);
    return pow;
  }

  function computeLedArray(leds: IColoredPoint2D[], sun: ISun): IColoredPoint2D[] {
    return leds.map(led => {
      return { x: led.x, y: led.y, hsl: getHslLedColor(led, sun) };
    });
  }

  function getHslLedColor(
    led: IColoredPoint2D,
    sun: ISun
  ): Hsl {
    const dist = Math.abs(getDistanceBetweenTwoPoints(sun, led));
    const invertedDist = dist / maxDistance;
    const l = (1 - invertedDist) * sun.strength;
    return { h: 360, s: 1, l };
  }

  function toHslString({ h, s, l }: Hsl) {
    return `hsl(${h},${s * 100}%,${l * 100}%)`;
  }
}

const canvas = document.getElementById("canvas") as HTMLCanvasElement;
const ctx3 = canvas.getContext("2d");
if (ctx3 != null) go(ctx3);
