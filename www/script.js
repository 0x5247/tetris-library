let wasmLogger = null;
let wasmPanicHandler = null;

WebAssembly.instantiateStreaming(fetch("/mod.wasm"), {
	env: {
		js_log: (ptr, len) => wasmLogger(ptr, len),
		js_panic: (ptr, len) => wasmPanicHandler(ptr, len),
	},
}).then((mod) => {
	const exports = mod.instance.exports;
	const memory = exports.memory;

	const dec = new TextDecoder();

	wasmLogger = (ptr, len) => {
		const logBytes = new Uint8Array(memory.buffer, ptr, len);
		console.log(dec.decode(logBytes));
	};

	wasmPanicHandler = (ptr, len) => {
		const msg = dec.decode(new Uint8Array(memory.buffer, ptr, len));

		console.error(`wasm panic: ${msg}`);
		throw new Error(msg);
	};

	const ctx = canvas.getContext("2d", { willReadFrequently: true });

	const pixels = exports.get_frame_buffer();

	const width = exports.get_width();
	const height = exports.get_height();

	canvas.width = width;
	canvas.height = height;

	const size = width * height * 4;

	const frame = new ImageData(new Uint8ClampedArray(memory.buffer, pixels, size), width, height);

	const buttons = [btnRotate, btnLeft, btnRight, btnDown, btnDrop];
	const keyCodes = ["KeyW", "KeyA", "KeyD", "KeyS", "KeySpace"];

	btnContainer.addEventListener("touchstart", () => {
		btnContainer.firstElementChild.remove();

		buttons.forEach((btn) => btn.style.display = "inline-block");

		btnContainer.addEventListener("touchstart", (e) => {
			const input = buttons.indexOf(e.target);
			if (input != -1) {
				exports.input_enqueue(input);
			}
		});
	}, { once: true });

	window.addEventListener("keydown", () => {
		btnContainer.remove();

		window.addEventListener("keydown", (e) => {
			const input = keyCodes.indexOf(e.code);
			if (input != -1) {
				exports.input_enqueue(input);
			}
		});
	}, { once: true });

	const DT = 1.0/60.0;
	let pt = undefined;

	const loop = (t) => {
		let dt = (t - pt) * 0.001;
		pt = t;

		const steps = dt / DT;

		for (let i = 0; i < steps; i += 1) {
			exports.update(DT);
		}

		exports.update(dt - (DT * steps));

		exports.render();

		ctx.putImageData(frame, 0, 0);

		requestAnimationFrame(loop);
	};

	exports.init();

	requestAnimationFrame((t) => {
		pt = t;

		exports.update(0.0);
		exports.render();

		ctx.putImageData(frame, 0, 0);
		requestAnimationFrame(loop);
	});
});
