const fs = require("fs");

function getRandomInt(min, max) {
	min = Math.ceil(min);
	max = Math.floor(max);
	return Math.floor(Math.random() * (max - min) + min);
}

function generate_rotor_values(basestart, basestop, filename)
{
	var data = '';
	for (let i = basestart; i < basestop; i++)
		data += String.fromCharCode(i);
	data = data.split('').sort(function(){return 0.5-Math.random()}).join(''); // for random shuflle
	fs.writeFileSync(filename, data);
	console.log(`${filename} done`);
}

function generate_refclector(basestart, basestop, filename)
{
	var data = '';
	for (let i = basestart; i < basestop; i++)
		data += String.fromCharCode(i);
	let prev = 0;
	let r = getRandomInt(3, (basestop - basestart) / 5);
	while (prev + r < (basestop - basestart))
	{
		if (r % 2 == 0)
			r -= 1;
		data = data.slice(0, prev) + data.slice(prev, prev + r).split('').reverse().join('') + data.slice(prev + r, (basestop - basestart));
		prev += r;
		r = getRandomInt(2, (basestop - basestart) / 5);
	}
	fs.writeFileSync(filename, data);
	console.log(`${filename} done`);
}

// const BASESTART = 65 // indexes
// const BASESTOP = 91

const BASESTART = 0 // indexes
const BASESTOP = 256

generate_rotor_values(BASESTART, BASESTOP, "rotor1");
generate_rotor_values(BASESTART, BASESTOP, "rotor2");
generate_rotor_values(BASESTART, BASESTOP, "rotor3");

generate_refclector(BASESTART, BASESTOP, "reflector");
generate_refclector(BASESTART, BASESTOP, "plugboard");
