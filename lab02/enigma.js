const fs = require('fs');
const { assert } = require('console');

class Rotor {
	constructor(filename, pos = 0)
	{
		this.values = fs.readFileSync(filename).toString().split(',');
		this.values.forEach((item, index, array) => array[index] = parseInt(item));
		this.base = this.values.length;
		this.position = pos;
		this.turnoverpos = 0;
	}

	turn()
	{
		this.position ++;
		if (this.position == this.base)
			this.position = 0;
		if (this.position == this.turnoverpos)
			return 1;
		return 0;
	}

	getByte(index)
	{
		index -= this.position;
		if (index < 0)
			index += this.base;
		return(this.values[index]);
	}

	getIndex(byte)
	{
		var i =  this.values.indexOf(byte) + this.position;
		if (i >= this.base)
			i -= this.base;
		return(i);
	}
}

class Reflector {
	constructor(filename)
	{
		this.values = fs.readFileSync(filename).toString().split(',');
		this.values.forEach((item, index, array) => array[index] = parseInt(item));
		this.base = this.values.length;
	}

	getPair(byte)
	{
		return (this.values[byte]);
	}
}

class Plugboard {
	constructor(filename)
	{
		this.values = fs.readFileSync(filename).toString().split(',');
		this.values.forEach((item, index, array) => array[index] = parseInt(item));
		this.base = this.values.length;
	}

	getPair(byte)
	{
		return (this.values[byte]);
	}
}

class Enigma {
	constructor(rotor_confs, init_pos, ref_conf, plb_conf)
	{
		assert(rotor_confs.length == init_pos.length);
		this.number_of_rotors = rotor_confs.length;
		this.rotors = []
		for (let i = 0; i < this.number_of_rotors; i++)
		{
			this.rotors.push(new Rotor(rotor_confs[i], init_pos[i]));
			if (i != 0)
				assert(this.rotors[i].base == this.rotors[i - 1].base);
		}
		this.reflector = new Reflector(ref_conf);
		this.plugboard = new Plugboard(plb_conf);
	}

	codebyte(byte)
	{
		let i = 0;
		while (i < this.number_of_rotors && this.rotors[i].turn() == 1)
			i++;
		byte = this.plugboard.getPair(byte);
		for (let i = 0; i < this.number_of_rotors; i++)
			byte = this.rotors[i].getByte(byte);
		byte = this.reflector.getPair(byte);
		for (let i = this.number_of_rotors - 1; i >= 0; i--)
			byte = this.rotors[i].getIndex(byte);
		byte = this.plugboard.getPair(byte);
		return byte;
	}

	codefile(msg_file, res_file)
	{
		let msg = fs.readFileSync(msg_file);
		for (let i = 0; i < msg.length; i++)
			msg[i] = this.codebyte(msg[i]);
		fs.writeFileSync(res_file, msg);
	}
}

const enigma1 = new Enigma(['rotor1', 'rotor2', 'rotor3'], [0, 0, 0], 'reflector', 'plugboard');
enigma1.codefile('msg', "res");

const enigma2 = new Enigma(['rotor1', 'rotor2', 'rotor3'], [0, 0, 0], 'reflector', 'plugboard');
enigma2.codefile('res', "res_msg");
