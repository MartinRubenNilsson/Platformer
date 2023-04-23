#pragma once

struct EntityRef
{
	size_t entityIid = 0;
	size_t levelIid = 0;
};

struct Exit : EntityRef {};