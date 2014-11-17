﻿#include "pch.h"
#include "DataManager.h"
#include "cocos2d.h"
#include "json/json.h"


void DataManager::initWorldData()
{
	m_StageDatas.clear();

	for (int floor = 0; floor < 4; floor++)
	{
		StageData stage;

		initStageData(stage, floor, 8 + floor * 2 + rand() % (floor + 1));
	}
}

void DataManager::initStageData(StageData& stage, int floor, int roomNumber)
{
	stage.Rooms.clear();

	for (int idx = 0; idx < roomNumber; idx++)
	{
		RoomData room;

		initRoomData(room);

		stage.Rooms.push_back(room);
	}

	initRoomPlace(stage, floor);

	m_StageDatas.push_back(stage);

	//방 연결 정보 생성
	makeRoomConnectData(stage, floor);
	m_StageDatas[floor].Rooms = stage.Rooms;
}

void DataManager::initRoomData(RoomData& room)
{
	ModulePlaceType mpt = (ModulePlaceType)(rand() % MPT_NUM);

	initModulePlace(room, mpt);
}

void DataManager::fillRoomData(RoomData& room, int floor)
{

	room.data.clear();

	room.data.resize(room.width*room.height);

	cocos2d::Size sizeByModule;

	sizeByModule.width = room.width / m_ModuleSize.width;
	sizeByModule.height = room.height / m_ModuleSize.height;

	for (int y = 0; y < sizeByModule.height; y++)
	{
		for (int x = 0; x < sizeByModule.width; x++)
		{
			//모듈이 배치된 칸만 찾아서 값 채워넣는다.
			if (room.modulePlaceData[y*sizeByModule.width + x] == 0)
				continue;

			Direction dir = getModuleType(room, x, y);

			matchModuleData(room, dir, x, y, floor);
		}
	}

}

void DataManager::initRoomPlace(StageData& stage, int floor)
{
	int placeData[PLACEMAP_SIZE][PLACEMAP_SIZE] = { 0, };
	cocos2d::Point position;
	std::vector<cocos2d::Point> candidate;
	cocos2d::Size sizeByModule;
	cocos2d::Point minPos, maxPos;

	sizeByModule.width = stage.Rooms[0].width / m_ModuleSize.width;
	sizeByModule.height = stage.Rooms[0].height / m_ModuleSize.height;


	memset(m_PlaceData[floor], 0, sizeof(int)*PLACEMAP_SIZE*PLACEMAP_SIZE);

	//초기화로 첫번째 방의 위치를 랜덤하게 정한다. 혹 첫번째 방을 고정시켜야할 경우 이 부분 수정하면 됨.
	stage.Rooms[0].x = rand() % (int)(100 - sizeByModule.width);
	stage.Rooms[0].y = rand() % (int)(100 - sizeByModule.height);
	minPos.x = stage.Rooms[0].x;
	minPos.y = stage.Rooms[0].y;
	maxPos.x = stage.Rooms[0].x + sizeByModule.width;
	maxPos.y = stage.Rooms[0].y + sizeByModule.height;

	setPlaceData(placeData, stage.Rooms[0], 0);

	//첫번째 방의 배치를 바탕으로 다른 방들도 랜덤하게 배치한다.

	for (int idx = 1; idx < stage.Rooms.size(); idx++)
	{
		sizeByModule.width = stage.Rooms[idx].width / m_ModuleSize.width;
		sizeByModule.height = stage.Rooms[idx].height / m_ModuleSize.height;

		candidate.clear();
		int startX = minPos.x - sizeByModule.width - 2;
		int startY = minPos.y - sizeByModule.height - 2;

		if (startX < 0)
			startX = 0;

		if (startY < 0)
			startY = 0;

		//가능한 후보군을 선정한 후 이 중에서 랜덤하게 택일한다.
		for (int y = startY; y <= maxPos.y; y++)
		{
			for (int x = startX; x <= maxPos.x; x++)
			{
				if (isCandidatePos(placeData, x, y, stage.Rooms[idx]))
					candidate.push_back(cocos2d::Point(x, y));
			}
		}

		int targetIdx = rand() % candidate.size();

		stage.Rooms[idx].x = candidate[targetIdx].x;
		stage.Rooms[idx].y = candidate[targetIdx].y;

		setPlaceData(placeData, stage.Rooms[idx], idx);

		if (minPos.x > stage.Rooms[idx].x)
		{
			minPos.x = stage.Rooms[idx].x;
		}

		if (minPos.y > stage.Rooms[idx].y)
		{
			minPos.y = stage.Rooms[idx].y;
		}

		if (maxPos.x < stage.Rooms[idx].x + sizeByModule.width)
		{
			maxPos.x = stage.Rooms[idx].x + sizeByModule.width;
		}

		if (maxPos.y < stage.Rooms[idx].y + sizeByModule.height)
		{
			maxPos.y = stage.Rooms[idx].y + sizeByModule.height;
		}

		if (maxPos.x >= PLACEMAP_SIZE)
		{
			maxPos.x = PLACEMAP_SIZE;
		}

		if (maxPos.y >= PLACEMAP_SIZE)
		{
			maxPos.y = PLACEMAP_SIZE;
		}
	}

	//평행이동
	for (int idx = 0; idx < stage.Rooms.size(); idx++)
	{
		stage.Rooms[idx].x -= minPos.x;
		stage.Rooms[idx].y -= minPos.y;
		stage.Rooms[idx].x *= m_ModuleSize.width;
		stage.Rooms[idx].y *= m_ModuleSize.height;
	}

	//0,0에 딱 붙여서 placeData 저장.
	for (int y = minPos.y; y < PLACEMAP_SIZE; y++)
	{
		for (int x = minPos.x; x < PLACEMAP_SIZE; x++)
		{
			m_PlaceData[floor][(int)(y - minPos.y)][(int)(x - minPos.x)] = placeData[y][x];
		}
	}

	//width,height 계산
	int maxX = 0, maxY = 0;
	for (int i = 0; i < stage.Rooms.size(); i++)
	{
		if (maxX < stage.Rooms[i].x + stage.Rooms[i].width)
			maxX = stage.Rooms[i].x + stage.Rooms[i].width;

		if (maxY < stage.Rooms[i].y + stage.Rooms[i].height)
			maxY = stage.Rooms[i].y + stage.Rooms[i].height;
	}
	stage.width = maxY + 1;
	stage.height = maxX + 1;
}

void DataManager::initRoomPlace(int floor)
{
	initRoomPlace(m_StageDatas[floor], floor);
}

void DataManager::initModulePlace(RoomData& room, ModulePlaceType mpt)
{
	cocos2d::Size size;
	int num;

	room.modulePlaceData.clear();

	switch (mpt)
	{
	case MPT_RECT:
		size.width = 1 + rand() % 3;
		size.height = 1 + rand() % 3;
		initModulePlaceByRect(room.modulePlaceData, size);
		break;
	case MPT_DOUGHNUT:
		size.width = 3 + rand() % 2;
		size.height = 3 + rand() % 2;
		initModulePlaceByDoughnut(room.modulePlaceData, size);
		break;
	case MPT_RANDOM:
		size.width = 2 + rand() % 5;
		size.height = 2 + rand() % 5;
		num = (size.width / 2)*(size.height / 2) + 2;
		initModulePlaceByRandom(room.modulePlaceData, size, num);
		break;
	}
	room.width = size.width * m_ModuleSize.width;
	room.height = size.height * m_ModuleSize.height;
}

void DataManager::initModulePlaceByRect(std::vector<int>& modulePlace, cocos2d::Size size)
{
	modulePlace.resize(size.height*size.width);

	for (int y = 0; y < size.height; y++)
	{
		for (int x = 0; x < size.width; x++)
		{
			modulePlace[y*size.width + x] = 1;
		}
	}
}

void DataManager::initModulePlaceByDoughnut(std::vector<int>& modulePlace, cocos2d::Size size)
{
	modulePlace.resize(size.height*size.width);

	for (int y = 0; y < size.height; y++)
	{
		for (int x = 0; x < size.width; x++)
		{
			if (y == 0 || y == size.height - 1 ||
				x == 0 || x == size.width - 1)
				modulePlace[y*size.width + x] = 1;
		}
	}
}

void DataManager::initModulePlaceByRandom(std::vector<int>& modulePlace, cocos2d::Size size, int moduleNum)
{
	modulePlace.resize(size.height*size.width);

	cocos2d::Point pos;

	pos.x = rand() % (int)size.width;
	pos.y = rand() % (int)size.height;

	modulePlace[pos.y*size.width + pos.x] = 1;

	for (int i = 1; i < moduleNum; i++)
	{

		cocos2d::Point nextPos;

		do
		{
			//1이 있는 위치 랜덤 선택.
			do
			{
				pos.x = rand() % (int)size.width;
				pos.y = rand() % (int)size.height;
			} while (pos.x < 0 || pos.x >= size.width ||
				pos.y < 0 || pos.y >= size.height ||
				modulePlace[pos.y*size.width + pos.x] == 0);

			int dir = rand() % 4;

			nextPos = pos;

			switch (dir)
			{
			case 0: // 위쪽
				nextPos.y++;
				break;
			case 1: // 오른쪽
				nextPos.x++;
				break;
			case 2: //아래쪽
				nextPos.y--;
				break;
			case 3: //왼쪽
				nextPos.x--;
				break;
			}
		} while (nextPos.x < 0 || nextPos.x >= size.width ||
			nextPos.y < 0 || nextPos.y >= size.height ||
			modulePlace[nextPos.y*size.width + nextPos.x] == 1); //적절한 빈칸인 경우 해당 포지션에 모듈 배치.

		modulePlace[nextPos.y*size.width + nextPos.x] = 1;
	}
}

void DataManager::matchModuleData(RoomData& room, int type, int startX, int startY, int floor)
{
	int idx;
	int tileX = startX * m_ModuleSize.width;
	int tileY = startY * m_ModuleSize.height;
	int portalDir = DIR_NONE;
	portalDir = isPortal(floor, (room.x + tileX) / m_ModuleSize.width, (room.y + tileY) / m_ModuleSize.height);

	do
	{
		idx = rand() % m_ModuleDatas[type].size();
	} while ((!!portalDir) ^ isPortalType(type, idx));

	

	for (int y = 0; y < m_ModuleSize.height; y++)
	{
		for (int x = 0; x < m_ModuleSize.width; x++)
		{
			int blockRandom = rand() % 100;
			int floorRandom = rand() % 100;
			ComponentType data = CT_NONE;
			switch ((ComponentType)m_ModuleDatas[type][idx].data[y*m_ModuleSize.width + x])
			{
			case RT_BLOCK:
				data = OT_BLOCK;
				break;
			case RT_FLOOR:
				data = OT_FLOOR;
				break;
			case RT_PORTAL:
				if ((x == 0 && m_PlaceData[startY][startX - 1] != 0 && (portalDir & DIR_LEFT)) ||
					(x == m_ModuleSize.width - 1 && m_PlaceData[startY][startX + 1] != 0 && (portalDir & DIR_RIGHT)) ||
					(y == 0 && m_PlaceData[startY - 1][startX] != 0 && (portalDir & DIR_DOWN)) ||
					(y == m_ModuleSize.height - 1 && m_PlaceData[startY + 1][startX] != 0 && (portalDir & DIR_UP)))
					data = OT_PORTAL_OPEN;
				else
					data = OT_BLOCK;
				break;
			case RT_BLOCK_RANDOM:
				if (blockRandom < 70)
				{
					data = OT_BLOCK;
				}
				break;
			case RT_FLOOR_RANDOM:
				if (floorRandom < 70)
				{
					data = OT_FLOOR;
				}
				break;
			case RT_MONSTER_RANDOM:
				if (rand() % 100 < 70)
				{
					data = OT_MONSTER_STAND_SHOT;
				}
				break;
			}
			room.data[(tileY + y)*room.width + tileX + x] = data;
		}
	}
}

bool DataManager::isCandidatePos(int placeData[PLACEMAP_SIZE][PLACEMAP_SIZE], int x, int y, RoomData& room)
{
	bool isConnected = false;
	cocos2d::Size sizeByModule;

	sizeByModule.width = room.width / m_ModuleSize.width;
	sizeByModule.height = room.height / m_ModuleSize.height;

	for (int rx = 0; rx < sizeByModule.width; rx++)
	{
		for (int ry = 0; ry < sizeByModule.height; ry++)
		{
			int ridx = ry*sizeByModule.width + rx;
			if (room.modulePlaceData[ridx] == 1)
			{
				//room이 배치되어야 하는 곳은 비어있어야함.
				if (placeData[y + ry][x + rx] != 0)
					return false;

				//연결 체크 끝났으면 생략.
				if (isConnected)
					continue;

				//해당 칸 주변 4칸이 비지 않았으면 연결된 방.

				//왼쪽
				if (x + rx >= 1 && placeData[y + ry][x + rx - 1] != 0)
					isConnected = true;

				//오른쪽
				if (x + rx < PLACEMAP_SIZE - 1 && placeData[y + ry][x + rx + 1] != 0)
					isConnected = true;

				//위
				if (y + ry >= 1 && placeData[y + ry - 1][x + rx] != 0)
					isConnected = true;

				//아래
				if (y + ry < PLACEMAP_SIZE - 1 && placeData[y + ry + 1][x + rx] != 0)
					isConnected = true;
			}
		}
	}

	return isConnected;
}

void DataManager::makeRoomConnectData(StageData& stage, int floor)
{
	for (int i = 0; i < stage.Rooms.size(); i++)
	{
		makePortal(stage.Rooms[i], floor, i);
		fillRoomData(stage.Rooms[i], floor);
	}
}

void DataManager::makePortal(RoomData& room, int floor, int idx)
{
	int rx = room.x / m_ModuleSize.width;
	int ry = room.y / m_ModuleSize.height;
	int width = room.width / m_ModuleSize.width;
	int height = room.height / m_ModuleSize.height;

	std::vector<std::vector<PortalData>> portalCandidates;

	portalCandidates.resize(m_StageDatas[floor].Rooms.size() + 1);

	for (int y = ry; y < ry + height; y++)
	{
		for (int x = rx; x < rx + width; x++)
		{
			int ridx = (y - ry)*width + x - rx;

			//모듈이 있는 경우만 판단
			if (room.modulePlaceData[ridx] == 0)
			{
				continue;
			}

			int dir = getConnectedDirections(room, floor, x, y);
			PortalData portal;
			portal.pos = cocos2d::Point(x, y);
			portal.roomIdx[0] = idx + 1;

			//위쪽 방향 검사
			if (dir & DIR_UP)
			{
				int nextRoom = m_PlaceData[floor][y + 1][x];

				if (nextRoom > idx + 1)
				{
					portal.roomIdx[1] = nextRoom;
					portal.dir = DIR_UP;
				}
				portalCandidates[nextRoom].push_back(portal);
			}

			//오른쪽 방향 검사
			if (dir & DIR_RIGHT)
			{
				int nextRoom = m_PlaceData[floor][y][x + 1];

				if (nextRoom > idx + 1)
				{
					portal.roomIdx[1] = nextRoom;
					portal.dir = DIR_RIGHT;
				}
				portalCandidates[nextRoom].push_back(portal);
			}

			//아래쪽 방향 검사
			if (dir & DIR_DOWN)
			{
				int nextRoom = m_PlaceData[floor][y - 1][x];

				if (nextRoom > idx + 1)
				{
					portal.roomIdx[1] = nextRoom;
					portal.dir = DIR_DOWN;
				}
				portalCandidates[nextRoom].push_back(portal);
			}

			//왼쪽 방향 검사
			if (dir & DIR_LEFT)
			{
				int nextRoom = m_PlaceData[floor][y][x - 1];

				if (nextRoom > idx + 1)
				{
					portal.roomIdx[1] = nextRoom;
					portal.dir = DIR_LEFT;
				}
				portalCandidates[nextRoom].push_back(portal);
			}
		}
	}

	for (int nextRoomIdx = idx + 2; nextRoomIdx < portalCandidates.size(); nextRoomIdx++)
	{
		if (portalCandidates[nextRoomIdx].size() == 0)
		{
			continue;
		}

		int portalIdx = rand() % portalCandidates[nextRoomIdx].size();
		PortalData portal = portalCandidates[nextRoomIdx][portalIdx];
		cocos2d::Point nextPos = portal.pos;
		int nextDir;

		m_StageDatas[floor].portals.push_back(portal);

		portal.roomIdx[0] = portal.roomIdx[1];
		portal.roomIdx[1] = idx + 1;

		if (portal.dir == DIR_UP)
		{
			nextPos.y++;
			nextDir = DIR_DOWN;
		}
		if (portal.dir == DIR_DOWN)
		{
			nextPos.y--;
			nextDir = DIR_UP;
		}

		if (portal.dir == DIR_RIGHT)
		{
			nextPos.x++;
			nextDir = DIR_LEFT;
		}

		if (portal.dir == DIR_LEFT)
		{
			nextPos.x--;
			nextDir = DIR_RIGHT;
		}

		portal.pos = nextPos;
		portal.dir = nextDir;

		m_StageDatas[floor].portals.push_back(portal);
	}
}

int DataManager::getConnectedDirections(RoomData& room, int floor, int x, int y)
{

	//rx,ry는 room 기준 좌표.
	//x,y는 전체 world 기준 좌표.
	int rx = x - room.x / m_ModuleSize.width;
	int ry = y - room.y / m_ModuleSize.height;
	int width = room.width / m_ModuleSize.width;
	int height = room.height / m_ModuleSize.height;
	int ridx = ry*width + rx;
	int dir = DIR_NONE;

	//모듈이 있는 경우 어디어디로 연결되는지 파악한다.

	int connectIdx;
	int placeValue;

	//왼쪽 연결 테스트
	connectIdx = ry*width + rx - 1;
	placeValue = (rx > 0) ? room.modulePlaceData[connectIdx] : 0;
	//왼쪽 칸이 자기는 없는 모듈이고 딴 애 모듈은 있는 경우.
	if (x > 0 &&
		placeValue == 0 && m_PlaceData[floor][y][x - 1] != 0)
	{
		dir |= DIR_LEFT;
	}

	//오른쪽 연결 테스트
	connectIdx = ry*width + rx + 1;
	placeValue = (rx < width - 1) ? room.modulePlaceData[connectIdx] : 0;
	//오른쪽 칸이 자기는 없는 모듈이고 딴 애 모듈은 있는 경우.
	if (x < PLACEMAP_SIZE - 1 &&
		placeValue == 0 && m_PlaceData[floor][y][x + 1] != 0)
	{
		dir |= DIR_RIGHT;
	}

	//아래쪽 연결 테스트
	connectIdx = (ry - 1)*width + rx;
	placeValue = (ry > 0) ? room.modulePlaceData[connectIdx] : 0;
	//아래쪽 칸이 자기는 없는 모듈이고 딴 애 모듈은 있는 경우.
	if (y > 0 &&
		placeValue == 0 && m_PlaceData[floor][y - 1][x] != 0)
	{
		dir |= DIR_DOWN;
	}

	//위쪽 연결 테스트
	connectIdx = (ry + 1)*width + rx;
	placeValue = (ry < height - 1) ? room.modulePlaceData[connectIdx] : 0;
	//위쪽 칸이 자기는 없는 모듈이고 딴 애 모듈은 있는 경우.
	if (y < PLACEMAP_SIZE - 1 &&
		placeValue == 0 && m_PlaceData[floor][y + 1][x] != 0)
	{
		dir |= DIR_UP;
	}

	return dir;
}

int DataManager::getModuleType(RoomData& room, int x, int y)
{
	Direction dir = DIR_NONE;
	cocos2d::Size sizeByModule;

	sizeByModule.width = room.width / m_ModuleSize.width;
	sizeByModule.height = room.height / m_ModuleSize.height;

	//아래 칸이 빈 경우
	if (y == 0 || room.modulePlaceData[(y - 1)*sizeByModule.width + x] == 0)
		dir |= DIR_DOWN;

	//위 칸이 빈 경우
	if (y == sizeByModule.height - 1 || room.modulePlaceData[(y + 1)*sizeByModule.width + x] == 0)
		dir |= DIR_UP;

	//왼쪽 칸이 빈 경우
	if (x == 0 || room.modulePlaceData[y*sizeByModule.width + x - 1] == 0)
		dir |= DIR_LEFT;

	//오른쪽 칸이 빈 경우
	if (x == sizeByModule.width - 1 || room.modulePlaceData[y*sizeByModule.width + x + 1] == 0)
		dir |= DIR_RIGHT;

	return dir;
}

void DataManager::setRoomData(RoomData& room, int sx, int sy, int ex, int ey, ComponentType type)
{
	for (int y = sy; y <= ey; y++)
	{
		for (int x = sx; x <= ex; x++)
		{
			int idx = y*room.width + x;

			room.data[idx] = type;
		}
	}
}

int DataManager::getNextRoomNumber(int floor, int room, cocos2d::Point& playerPos)
{
	//전체 월드에서 타일 기준으로 x,y좌표.
	int tileX = m_StageDatas[floor].Rooms[room].x + playerPos.x / m_TileSize.width;
	int tileY = m_StageDatas[floor].Rooms[room].y + playerPos.y / m_TileSize.height;

	//현재 플레이어가 있는 좌표의 모듈 기준 값.
	int moduleX = tileX / m_ModuleSize.width;
	int moduleY = tileY / m_ModuleSize.height;
	int nextRoom;

	//다음 방이 없는 경우(잘못된 접근)
	_ASSERT(m_PlaceData[floor][moduleY][moduleX] != 0);
	if (m_PlaceData[floor][moduleY][moduleX] == 0)
	{
		return -1;
	}

	nextRoom = m_PlaceData[floor][moduleY][moduleX] - 1;

	//현재 방의 전체 월드에서의 타일 기준 x,y 시작 좌표.
	int roomStartX = m_StageDatas[floor].Rooms[room].x*m_TileSize.width;
	int roomStartY = m_StageDatas[floor].Rooms[room].y*m_TileSize.width;

	//다음 방의 전체 월드에서의 타일 기준 x,y 시작 좌표.
	int nextRoomStartX = m_StageDatas[floor].Rooms[nextRoom].x*m_TileSize.width;
	int nextRoomStartY = m_StageDatas[floor].Rooms[nextRoom].y*m_TileSize.width;

	//현재 룸 내에서 플레이어의 좌표.
	cocos2d::Point playerPosInRoom;
	playerPosInRoom.x = playerPos.x + roomStartX;
	playerPosInRoom.y = playerPos.y + roomStartY;

	//새로 이동한 방에서의 플레이어 좌표.
	playerPos.x = playerPosInRoom.x - nextRoomStartX;
	playerPos.y = playerPosInRoom.y - nextRoomStartY;

	return nextRoom;
}

const RoomData& DataManager::getRoomData(int floor, int room)
{
	_ASSERT(!(floor < 0 || floor >= m_StageDatas.size() ||
		room < 0 || room >= m_StageDatas[floor].Rooms.size()));

	if (floor < 0 || floor >= m_StageDatas.size() ||
		room < 0 || room >= m_StageDatas[floor].Rooms.size())
		return RoomData();

	return m_StageDatas[floor].Rooms[room];
}


void DataManager::setPlaceData(int placeData[PLACEMAP_SIZE][PLACEMAP_SIZE], RoomData& room, int roomIdx)
{
	cocos2d::Size sizeByModule;

	sizeByModule.width = room.width / m_ModuleSize.width;
	sizeByModule.height = room.height / m_ModuleSize.height;

	for (int y = room.y; y < room.y + sizeByModule.height; y++)
	{
		for (int x = room.x; x < room.x + sizeByModule.width; x++)
		{
			if (placeData[y][x] == 0)
			{
				int ridx = (y - room.y)*sizeByModule.width + x - room.x;

				if (room.modulePlaceData[ridx] != 0)
				{
					placeData[y][x] = roomIdx + 1;
				}
			}
		}
	}
}

int DataManager::isPortal(int floor, int x, int y)
{
	int dir = DIR_NONE;
	for (auto& portal : m_StageDatas[floor].portals)
	{
		if (portal.pos.x == x && portal.pos.y == y)
		{
			dir |= portal.dir;
		}
	}

	return dir;
}

bool DataManager::isPortalType(int type, int idx)
{

	for (int x = 0; x < m_ModuleSize.width; x++)
	{
		if (m_ModuleDatas[type][idx].data[x] == RT_PORTAL ||
			m_ModuleDatas[type][idx].data[(m_ModuleSize.height-1)*m_ModuleSize.height + x] == RT_PORTAL)
			return true;
	}

	for (int y = 0; y < m_ModuleSize.height; y++)
	{
		if (m_ModuleDatas[type][idx].data[y*m_ModuleSize.height] == RT_PORTAL ||
			m_ModuleDatas[type][idx].data[(y)*m_ModuleSize.height + m_ModuleSize.width - 1] == RT_PORTAL)
			return true;
	}

	return false;
}
