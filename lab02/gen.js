const fs = require('fs')

function getRandomInt(min, max) {
	min = Math.ceil(min);
	max = Math.floor(max);
	return Math.floor(Math.random() * (max - min) + min);
}

function generate_rotor_values(filename)
{
	var data = [];
	const base = 256;
	for (let i = 0; i < base; i++)
		data.push(i);
	data = data.sort(function(){return 0.5-Math.random()}); // for random shuflle
	fs.writeFileSync(filename, data);
	console.log(`${filename} done`);
}

function generate_refclector(filename)
{
	const base = 256;
	var data = [];
	for (let i = 0; i < base; i++)
		data.push(i);
	let prev = 0;
	let r = getRandomInt(3, base / 5);
	while (prev + r <= base)
	{
		if (r % 2 != 0)
			r -= 1;
		data = data.slice(0, prev).concat(data.slice(prev, prev + r).reverse()).concat(data.slice(prev + r, base));
		prev += r;
		r = getRandomInt(2, base / 5);
	}
	fs.writeFileSync(filename, data);
	console.log(`${filename} done`);
}

generate_rotor_values("rotor1");
generate_rotor_values("rotor2");
generate_rotor_values("rotor3");

generate_refclector("reflector");
generate_refclector("plugboard");
