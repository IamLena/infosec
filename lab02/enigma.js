
const fs = require('fs')

class Rotor {
	constructor(filename)
	{
		this.values = fs.readFileSync(filename).toString();
		this.base = this.values.length;
		this.position = 0;
		this.turnoverpos = 0;
		// this.turnoverpos = Math.floor(Math.random() * (this.base - 1)) // random byte; should be saved in config too
	}

	getIndex(b)
	{
		var i =  this.values.indexOf(b) + this.position;
		if (i >= this.base)
			i -= this.base;
		return String.fromCharCode(i);
	}

	getByte(b)
	{
		let index = b.charCodeAt(0);
		index -=  this.position;
		if (index < 0)
			index += this.base;
		var i = this.values[index];
		if (i < 0)
			i += this.base;
		return i;
	}

	setInitPosition(index)
	{
		if (index >= 0 && index < this.base)
			this.position = index;
		else
			throw ("invalid initial position");
	}

	turn()
	{
		// console.log(`turning ${this.position}`);
		this.position ++;
		if (this.position == this.base)
			this.position = 0;
		if (this.position == this.turnoverpos)
			return 1;
		return 0;
	}
}

class Reflector {
	constructor(filename)
	{
		this.values = fs.readFileSync(filename).toString();
		this.base = this.values.length;
	}

	getPair(b)
	{
		let index = b.charCodeAt(0);
		var i = this.values[index];
		if (i < 0)
			i += this.base;
		return i;
	}
}

class Plugboard {
	constructor(filename)
	{
		this.values = fs.readFileSync(filename).toString();
		this.base = this.values.length;
	}

	getPair(b)
	{
		let index = b.charCodeAt(0);
		var i = this.values[index];
		if (i < 0)
			i += this.base;
		return i;
	}
}

class Enigma {
	constructor(rotor_confs, init_pos, ref_conf, plb_conf)
	{
		this.number_of_rotors = rotor_confs.length;
		this.rotors = []
		for (let i = 0; i < this.number_of_rotors; i++)
		{
			this.rotors.push(new Rotor(rotor_confs[i]));
			this.rotors[i].setInitPosition(init_pos[i])
		}
		this.reflector = new Reflector(ref_conf);
		this.plugboard = new Plugboard(plb_conf);
	}

	codechar(byte)
	{
		let i = 0;
		// console.log(`turning ${i} rotor`);
		while (i < this.number_of_rotors && this.rotors[i].turn() == 1)
		{
			i++;
			// console.log(`turning ${i} rotor`);
		}
		// console.log(`init = ${byte}`);
		byte = this.plugboard.getPair(byte);
		// console.log(`plug = ${byte}`);
		for (let i = 0; i < this.number_of_rotors; i++)
		{
			byte = this.rotors[i].getByte(byte);
			// console.log(`roto = ${byte}`);
		}
		byte = this.reflector.getPair(byte);
		// console.log(`refl = ${byte}`);
		for (let i = this.number_of_rotors - 1; i >= 0; i--)
		{
			byte = this.rotors[i].getIndex(byte);
			// console.log(`roto = ${byte}`);
		}
		byte = this.plugboard.getPair(byte);
		// console.log(`plug = ${byte}`);
		return byte;
	}

	codestring(msg)
	{
		let result = ''
		console.log(`data = ${msg}`);
		msg.split('').forEach(byte => {
			result += this.codechar(byte);
		});
		return (result)
	}

	codefile(filename)
	{
		let msg = fs.readFileSync(filename).toString();
		let result = ''
		console.log(`data = ${msg}`);
		msg.split('').forEach(byte => {
			result += this.codechar(byte);
		});
		return (result)
	}

}

// CONFIG
const ROTOR1_CONFIG_FILES = ["rotor1", "rotor2", "rotor3"]; //turn over pos - 1st symbol in config file
const INIT_POSITIONS = [254, 255, 0];
const REFLECTOR_CONFIG_FILE = "reflector";
const PLUGBOARD_CONFIG_FILE = "plugboard";

// WORK
try {

	// CODE
	const enigma1 = new Enigma(
		ROTOR1_CONFIG_FILES,
		INIT_POSITIONS,
		REFLECTOR_CONFIG_FILE,
		PLUGBOARD_CONFIG_FILE
	);

	const msg = "AAABBBCCC";
	const filemsg = "msg"

	// const data1 = enigma1.codestring(msg);
	const data1 = enigma1.codefile(filemsg);
	console.log(`data1 = ${data1}`)

	// DECODE

	const enigma2 = new Enigma(
		ROTOR1_CONFIG_FILES,
		INIT_POSITIONS,
		REFLECTOR_CONFIG_FILE,
		PLUGBOARD_CONFIG_FILE
	);
	const data2 = enigma2.codestring(data1);
	console.log(`data2 = ${data2}`)
}
catch(err) {
	console.log(err);
}
