globalThis.assert = (cond, message = null) => {
	if (!cond) {
		const obj = {};

		if ("captureStackTrace" in Error) {
			Error.captureStackTrace(obj, globalThis.assert);
		}

		throw new Error((message ?? "Assertion failed!") + "\n" + obj.stack);
	}
};

globalThis.onerror = (message, source, line, col, error) => {
	alert(`${message}\n${source}:${line}:${col}`);
	throw new Error(error);
};

const consoul = document.createElement("pre");

consoul.setAttribute("style", `
	position: fixed;
	bottom: 0;
	left: 0;
	right: 0;
	margin: 0;
	background-color: #0005;
	color: #fff;
	padding: 8px;
	max-height: 1em;
	font-size: 1em;
	z-index: 9999;

	max-height: 25%;
	overflow-y: auto;
`);

document.body.append(consoul);

const wrapConsoleMethod = (type) => {
	const originalMethod = globalThis.console[type];
	let cnt = 0;

	globalThis.console[type] = (...args) => {
		originalMethod.apply(console, args);

		let msg = `[${type} (${++cnt})]>>----\n`;

		for (let i = 0; i < args.length; i += 1) {
			msg += safeStringify(args[i]) + " ";
		}

		msg += `\n----<<[${type}]\n`;

		consoul.append(msg);
	};
}

wrapConsoleMethod`log`;
wrapConsoleMethod`warn`;
wrapConsoleMethod`error`;
wrapConsoleMethod`info`;

const isAnInstance = (obj) => {
	return Object.getPrototypeOf(Object.getPrototypeOf(obj)) !== null;
};

const safeStringify = (data) => {
	if (typeof data !== "object") {
		return `${data}`;
	}

	if (data && isAnInstance(data)) {
		// todo: find a better way to do this
		return `${data}`;
	}

	const seen = new WeakSet();
	return JSON.stringify(data, (key, value) => {
		if (typeof value === 'object' && value !== null) {
			if (seen.has(value)) {
				return '[Circular Reference]';
			}
			seen.add(value);
		}
		return value;
	}, 2);
}
