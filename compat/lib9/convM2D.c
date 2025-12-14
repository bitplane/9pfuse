#include	<u.h>
#include	<libc.h>
#include	<fcall.h>

int
statchecku(uchar *buf, uint nbuf, int dotu)
{
	uchar *ebuf;
	int i, nstr;
	uint fixlen;

	ebuf = buf + nbuf;
	fixlen = dotu ? STATFIXLENU : STATFIXLEN;

	if(nbuf < fixlen || nbuf != BIT16SZ + GBIT16(buf))
		return -1;

	/* Skip past fixed portion to the first string length field */
	/* Fixed portion minus the string length fields and .u numeric fields */
	buf += STATFIXLEN - 4 * BIT16SZ;

	nstr = dotu ? 5 : 4;
	for(i = 0; i < nstr; i++){
		if(buf + BIT16SZ > ebuf)
			return -1;
		buf += BIT16SZ + GBIT16(buf);
	}

	/* Skip .u numeric fields */
	if(dotu)
		buf += 3 * BIT32SZ;

	if(buf != ebuf)
		return -1;

	return 0;
}

int
statcheck(uchar *buf, uint nbuf)
{
	return statchecku(buf, nbuf, 0);
}

static char nullstring[] = "";

uint
convM2Du(uchar *buf, uint nbuf, Dir *d, char *strs, int dotu)
{
	uchar *p, *ebuf;
	char *sv[5];
	int i, ns, nstr;

	if(nbuf < STATFIXLEN)
		return 0;

	p = buf;
	ebuf = buf + nbuf;

	p += BIT16SZ;	/* ignore size */
	d->type = GBIT16(p);
	p += BIT16SZ;
	d->dev = GBIT32(p);
	p += BIT32SZ;
	d->qid.type = GBIT8(p);
	p += BIT8SZ;
	d->qid.vers = GBIT32(p);
	p += BIT32SZ;
	d->qid.path = GBIT64(p);
	p += BIT64SZ;
	d->mode = GBIT32(p);
	p += BIT32SZ;
	d->atime = GBIT32(p);
	p += BIT32SZ;
	d->mtime = GBIT32(p);
	p += BIT32SZ;
	d->length = GBIT64(p);
	p += BIT64SZ;

	nstr = dotu ? 5 : 4;
	for(i = 0; i < nstr; i++){
		if(p + BIT16SZ > ebuf)
			return 0;
		ns = GBIT16(p);
		p += BIT16SZ;
		if(p + ns > ebuf)
			return 0;
		if(strs){
			sv[i] = strs;
			memmove(strs, p, ns);
			strs += ns;
			*strs++ = '\0';
		}
		p += ns;
	}

	if(strs){
		d->name = sv[0];
		d->uid = sv[1];
		d->gid = sv[2];
		d->muid = sv[3];
		d->ext = dotu ? sv[4] : nullstring;
	}else{
		d->name = nullstring;
		d->uid = nullstring;
		d->gid = nullstring;
		d->muid = nullstring;
		d->ext = nullstring;
	}

	/* Parse .u numeric fields */
	if(dotu){
		if(p + 3*BIT32SZ > ebuf)
			return 0;
		d->uidnum = GBIT32(p);
		p += BIT32SZ;
		d->gidnum = GBIT32(p);
		p += BIT32SZ;
		d->muidnum = GBIT32(p);
		p += BIT32SZ;
	}else{
		d->uidnum = ~0;
		d->gidnum = ~0;
		d->muidnum = ~0;
	}

	return p - buf;
}

uint
convM2D(uchar *buf, uint nbuf, Dir *d, char *strs)
{
	return convM2Du(buf, nbuf, d, strs, 0);
}
